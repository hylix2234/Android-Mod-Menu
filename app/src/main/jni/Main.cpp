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

// Status fitur (diubah dari menu)
bool goldOn = false, mineOn = false, rangeOn = false;
// Nilai yang dipakai (bisa diubah lewat slider)
int goldValue = 999999999;
int mineValue = 99999;
float rangeValue = 50.0f;

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Toggle_Gold Banyak"),                 // featNum 0
            OBFUSCATE("Toggle_Gold per Tambang"),            // featNum 1
            OBFUSCATE("Toggle_Jangkauan Serang"),            // featNum 2
            OBFUSCATE("SeekBar_Nilai Jangkauan_1_200"),      // featNum 3
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
            goldOn = boolean;
            break;
        case 1:
            mineOn = boolean;
            break;
        case 2:
            rangeOn = boolean;
            break;
        case 3:
            rangeValue = (float) value;
            break;
        default:
            break;
    }
}

long (*old_get_Gold)();
long get_Gold() {
    if (goldOn) return (long) goldValue;
    return old_get_Gold();
}

int (*old_get_GoldPerMine)(void *instance);
int get_GoldPerMine(void *instance) {
    if (instance != nullptr && mineOn) return mineValue;
    return old_get_GoldPerMine(instance);
}

float (*old_get_AttackRange)(void *instance);
float get_AttackRange(void *instance) {
    if (instance != nullptr && rangeOn) return rangeValue;
    return old_get_AttackRange(instance);
}

void hack_thread() {
    while (!isLibraryLoaded(targetLibName)) {
        sleep(1);
    }

    HOOK(targetLibName, "0x230A908", get_Gold, old_get_Gold);
    HOOK(targetLibName, "0x20F4D1C", get_GoldPerMine, old_get_GoldPerMine);
    HOOK(targetLibName, "0x217D1CC", get_AttackRange, old_get_AttackRange);

    LOGI(OBFUSCATE("Done"));
}

__attribute__((constructor))
void lib_main() {
    std::thread(hack_thread).detach();
}
