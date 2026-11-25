#include <list>
#include <vector>
#include <cstring>
#include <pthread.h>
#include <thread>
#include <string>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.hpp"
#include "Menu/Menu.hpp"
#include "Menu/Jni.hpp"
#include "Includes/Macros.h"
#include "dobby.h"

// Status fitur
bool torqueOn = false;
float torqueMul = 1.5f;

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Category_Torsi"),
            OBFUSCATE("Toggle_Torque Boost"),                 // featNum 0
            OBFUSCATE("SeekBar_Torque Multiplier_10_500"),    // featNum 1 (1.0x sampai 50.0x)
    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

//Target main lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

void Changes(JNIEnv *env, jclass clazz, jobject obj, jint featNum, jstring featName, jint value, jlong Lvalue, jboolean boolean, jstring text) {

    switch (featNum) {
        case 0:
            torqueOn = boolean;
            break;
        case 1:
            torqueMul = value / 10.0f;   // 10..500 -> 1.0x..50.0x
            break;
        default:
            break;
    }
}

// Pengali torsi mesin (nilai asli dikali, bukan diganti)
float (*old_torque)(void *instance);
float new_torque(void *instance) {
    float orig = old_torque(instance);
    if (instance != nullptr && torqueOn) return orig * torqueMul;
    return orig;
}

void hack_thread() {
    while (!isLibraryLoaded(targetLibName)) {
        sleep(1);
    }

    HOOK(targetLibName, "0x7BDD710", new_torque, old_torque);

    LOGI(OBFUSCATE("Done"));
}

__attribute__((constructor))
void lib_main() {
    std::thread(hack_thread).detach();
}
