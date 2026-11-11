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
