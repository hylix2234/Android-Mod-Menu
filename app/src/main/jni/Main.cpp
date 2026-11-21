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
bool antiHackOn = false, crystalOn = false;
bool atkOn = false, defOn = false, cdOn = false;
// Nilai pengali (slider)
int crystalMul = 2;
int statMul = 2;
int cdMul = 2;
// Instance untuk UnlockAllFloor (ditangkap otomatis)
void *floorInstance = nullptr;

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Category_Aman"),
            OBFUSCATE("Toggle_Anti Deteksi Hack"),            // featNum 0
            OBFUSCATE("Category_Kristal"),
            OBFUSCATE("Toggle_Kristal Lebih Banyak"),         // featNum 1
            OBFUSCATE("SeekBar_Kristal Multiply_1_10000"),    // featNum 2
            OBFUSCATE("Category_Stat"),
            OBFUSCATE("Toggle_Serangan Naik"),                // featNum 3
            OBFUSCATE("Toggle_Pertahanan Naik"),              // featNum 4
            OBFUSCATE("SeekBar_Stat Multiply_1_1000"),        // featNum 5
            OBFUSCATE("Category_Cooldown"),
            OBFUSCATE("Toggle_Cooldown Skill Turun"),         // featNum 6
            OBFUSCATE("SeekBar_Cooldown Multiply_1_5"),       // featNum 7
            OBFUSCATE("Category_Lain"),
            OBFUSCATE("Button_Buka Semua Lantai"),            // featNum 8
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
            antiHackOn = boolean;
            break;
        case 1:
            crystalOn = boolean;
            break;
        case 2:
            crystalMul = value;
            break;
        case 3:
            atkOn = boolean;
            break;
        case 4:
            defOn = boolean;
            break;
        case 5:
            statMul = value;
            break;
        case 6:
            cdOn = boolean;
            break;
        case 7:
            cdMul = value;
            break;
        case 8:
            if (floorInstance != nullptr) {
                ((void (*)(void *)) getAbsoluteAddress(targetLibName, "0x195F26C"))(floorInstance);
            }
            break;
        default:
            break;
    }
}

// Anti deteksi hack kristal
bool (*old_IsHackCrystal)(void *instance);
bool IsHackCrystal(void *instance) {
    if (antiHackOn) return false;
    return old_IsHackCrystal(instance);
}

// Kristal masuk dikali pengali (hanya yang positif, dengan pengaman overflow)
void (*old_AddCrystal)(void *instance, int crystal, void *source, int reason);
void AddCrystal(void *instance, int crystal, void *source, int reason) {
    if (instance != nullptr && crystalOn && crystal > 0) {
        long long r = (long long) crystal * crystalMul;
        if (r > 2000000000LL) r = 2000000000LL;
        crystal = (int) r;
    }
    old_AddCrystal(instance, crystal, source, reason);
}

// Stat (return double)
double (*old_BonusAtk)(void *instance);
double BonusAtk(void *instance) {
    double v = old_BonusAtk(instance);
    if (instance != nullptr && atkOn) return v * statMul;
    return v;
}

double (*old_BonusDef)(void *instance);
double BonusDef(void *instance) {
    double v = old_BonusDef(instance);
    if (instance != nullptr && defOn) return v * statMul;
    return v;
}

// Cooldown (pengali sendiri, batas kecil)
double (*old_BonusCD)(void *instance);
double BonusCD(void *instance) {
    double v = old_BonusCD(instance);
    if (instance != nullptr && cdOn) return v * cdMul;
    return v;
}

// Tangkap instance untuk tombol Buka Semua Lantai
int (*old_GetHighestFloor)(void *instance);
int GetHighestFloor(void *instance) {
    if (instance != nullptr) floorInstance = instance;
    return old_GetHighestFloor(instance);
}

void hack_thread() {
    while (!isLibraryLoaded(targetLibName)) {
        sleep(1);
    }

    HOOK(targetLibName, "0x1F538E8", IsHackCrystal, old_IsHackCrystal);
    HOOK(targetLibName, "0x1969458", AddCrystal, old_AddCrystal);
    HOOK(targetLibName, "0x1FB51FC", BonusAtk, old_BonusAtk);
    HOOK(targetLibName, "0x1FB520C", BonusDef, old_BonusDef);
    HOOK(targetLibName, "0x1FB51EC", BonusCD, old_BonusCD);
    HOOK(targetLibName, "0x195E73C", GetHighestFloor, old_GetHighestFloor);

    LOGI(OBFUSCATE("Done"));
}

__attribute__((constructor))
void lib_main() {
    std::thread(hack_thread).detach();
}
