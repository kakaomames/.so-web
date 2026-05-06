#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>

// JNIダミー
void* dummy_vtable[300];
struct { void** functions; } dummy_env = { dummy_vtable };

// ALooper_pollAll の型定義
typedef int (*ALooper_pollAll_t)(int, int*, int*, void**);

int main(int argc, char *argv[]) {
    printf("============================================\n");
    printf("🚀 ATTACK PROGRAM: SYMBOL FORCE-LINK\n");
    printf("============================================\n");

    // 1. システムライブラリを「RTLD_GLOBAL」でロード
    // RTLD_GLOBAL を指定することで、後続の dlopen がこの中の関数を使えるようにする
    void* libandroid = dlopen("libandroid.so", RTLD_NOW | RTLD_GLOBAL);
    if (!libandroid) {
        // もし直パスで見つからない場合は、Android標準パスを試す
        libandroid = dlopen("/system/lib64/libandroid.so", RTLD_NOW | RTLD_GLOBAL);
    }

    if (libandroid) {
        printf("✅ libandroid.so: FOUND and LOADED GLOBAL\n");
        // ALooper_pollAll を無理やりシンボルテーブルに登録させる試み
        void* poll_ptr = dlsym(libandroid, "ALooper_pollAll");
        printf("📍 ALooper_pollAll ADDR: %p\n", poll_ptr);
    } else {
        printf("❌ CANNOT LOAD libandroid.so\n");
    }

    // 2. 本命のロード
    // ここで ALooper_pollAll が解決されるはずだ！
    const char* target = "/data/data/com.termux/files/home/mission_so/libcocos2dcpp.so";
    void* handle = dlopen(target, RTLD_NOW | RTLD_GLOBAL);
    
    if (!handle) {
        printf("❌ DLOPEN ERROR: %s\n", dlerror());
        // まだエラーが出るなら、ここが最終防衛線だ
        return 1;
    }

    printf("✅ SO LOAD SUCCESS!\n");
    void (*func)(void*, void*) = dlsym(handle, "gameNativeInit");
    for(int i=0; i<300; i++) dummy_vtable[i] = (void*)printf;

    printf("🔥 FIRING gameNativeInit...\n");
    if(func) func(&dummy_env, NULL);
    
    printf("\n✅ MISSION COMPLETE\n");
    dlclose(handle);
    return 0;
}
