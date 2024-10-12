#include <jni.h>
#include <iostream>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif // _WIN32

std::string getExecutableDir() {
    char buffer[1024];
#ifdef _WIN32
    GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
#else
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    }
#endif
    std::filesystem::path exePath(buffer);
    return exePath.parent_path().string();
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_image_file>" << std::endl;
        return 1;
    }

    const char* filePath = argv[1];

    // Get the directory of the executable
    std::string execDir = getExecutableDir();

    #ifdef _MSC_VER
        // Construct the base path for the java directory located at one level up for MSVC build
        std::filesystem::path javaDir = std::filesystem::path(execDir).parent_path() / "java";
    #else
        // If compiling with GCC or another compiler, the java directory is in the same directory as the executable
        std::filesystem::path javaDir = std::filesystem::path(execDir) / "java";
    #endif

    // Construct the classpath using the javaDir
    std::string classpathString =
        "-Djava.class.path=" + javaDir.string() + "/BioFormatsWrapper.jar;" +
        javaDir.string() + "/bioformats_package.jar;" +
        javaDir.string() + "/bio-formats-tools.jar;" +
        javaDir.string() + "/formats-gpl.jar;" +
        javaDir.string() + "/formats-api.jar;";

    // Convert std::string to const char*
    const char* classpathOption = classpathString.c_str();

     // JNI initialization
     JavaVMOption options[1];
     options[0].optionString = const_cast<char*>(classpathOption);

     JavaVMInitArgs vmArgs;
     vmArgs.version = JNI_VERSION_1_8; // or your installed JDK version
     vmArgs.nOptions = 1;
     vmArgs.options = options;
     vmArgs.ignoreUnrecognized = false;

     JavaVM *jvm;
     JNIEnv *env;
     jint result = JNI_CreateJavaVM(&jvm, reinterpret_cast<void**>(&env), &vmArgs);
     if (result != JNI_OK) {
         std::cerr << "Error creating JVM: " << result << std::endl;
         return 1;
     }
     else {
         std::cout << "JVM succesfully created." << std::endl;
     }

      jclass wrapperClass = env->FindClass("BioFormatsWrapper");
      if (wrapperClass == nullptr) {
          std::cerr << "Error: BioFormatsWrapper Class not found." << std::endl;
          jvm->DestroyJavaVM();
          return -1;
      }

      jmethodID constructor = env->GetMethodID(wrapperClass, "<init>", "()V");
      jobject wrapperInstance = env->NewObject(wrapperClass, constructor);

      jmethodID getMetadataMethod = env->GetMethodID(wrapperClass, "getMetadata", "(Ljava/lang/String;)Ljava/lang/String;");
      jstring filePathJava = env->NewStringUTF("/path/to/your/image/file");
      jstring metadata = (jstring)env->CallObjectMethod(wrapperInstance, getMetadataMethod, filePathJava);

      if (metadata != nullptr) {
          const char* metadataChars = env->GetStringUTFChars(metadata, nullptr);
          std::cout << metadataChars << std::endl;
          env->ReleaseStringUTFChars(metadata, metadataChars);
      }
      else {
          std::cerr << "Error retrieving metadata" << std::endl;
      }

      // Destroy the JVM and clean up
      jvm->DestroyJavaVM();
      return 0;
}