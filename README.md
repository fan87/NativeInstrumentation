![banner](assets/banner.png)

### ⚠️️ Supported OS
|             | x86 64 | x86 32 | ARM 64 | ARM 32 |
|-------------|--------|--------|--------|--------|
| Linux       | 🟢     | 🔴     | 🔴     | 🔴     |
| Windows     | 🟢     | 🔴     | 🔴     | 🔴     |
| MacOS / OSX | 🔴     | 🔴     | 🟢     | 🔴     |

**However, the code itself should still work for every variant**. I
just don't have a machine to build or even test the missing ones, and I'm not sure
if I can configure GitHub Actions to do it.

**If you know a GitHub Actions alternative for the missing variants, please let me know,**
although I probably won't pay money for this open-source project.

### Usage
The `NativeInstrumentation` class provided in this library simply
implements `java.lang.instrument.Instrumentation`, so most existing code that
uses Instrumentation can work out of the box.

"Most", because class retransform is always enabled, there's no way
to set `Can-Retransform-Classes` or `Can-Redefine-Classes` to false.


Another usage is to use it as `Unsafe`, you can invoke or call any method you want, even if it's an illegal 
reflection access. You can also set any fields, still, even if it's illegal reflection access, and,
you can also set a field to a value that it shouldn't be (i.e. set a String field to a File instance),
the JVM won't crash, but it will get confusing for sure. Feel free to poke around with it.


To create the instance of Instrumentation, do:

```java
Instrumentation instrumentation = new NativeInstrumentation();
```

To set fields, do

```java
NativeInstrumentation.setStaticField(...);
```

etc.

### Setup

Gradle (Kotlin)
```kotlin
repositories {
    maven("https://maven.pkg.github.com/fan87/NativeInstrumentation")
}

dependencies {
    // Library
    implementation("me.fan87:native-instrumentation:1.0-SNAPSHOT")
    // Native Code, include what your project supports
    runtimeOnly("me.fan87:native-instrumentation-native-linux-x86_64:1.0-SNAPSHOT")
    runtimeOnly("me.fan87:native-instrumentation-native-windows-x86_64:1.0-SNAPSHOT")
    runtimeOnly("me.fan87:native-instrumentation-native-osx-aarch64:1.0-SNAPSHOT")
}
```

```groovy
repositories {
    maven { url "https://maven.pkg.github.com/fan87/NativeInstrumentation" }
}

dependencies {
    // Library
    implementation "me.fan87:native-instrumentation:1.0-SNAPSHOT"
    // Native Code, include what your project supports
    runtimeOnly "me.fan87:native-instrumentation-native-linux-x86_64:1.0-SNAPSHOT"
    runtimeOnly "me.fan87:native-instrumentation-native-windows-x86_64:1.0-SNAPSHOT"
    runtimeOnly "me.fan87:native-instrumentation-native-osx-aarch64:1.0-SNAPSHOT"
}
```

Maven

```xml
<repositories>
    <repository>
        <id>native-instrumentation</id>
        <url>https://maven.pkg.github.com/fan87/NativeInstrumentation</url>
    </repository>
</repositories>

<dependencies>
    <dependency>
        <groupId>me.fan87</groupId>
        <artifactId>native-instrumentation</artifactId>
        <version>1.0-SNAPSHOT</version>
    </dependency>
    <dependency>
        <groupId>me.fan87</groupId>
        <artifactId>native-instrumentation-native-linux-x86_64</artifactId>
        <version>1.0-SNAPSHOT</version>
        <scope>runtime</scope>
    </dependency>
    <dependency>
        <groupId>me.fan87</groupId>
        <artifactId>native-instrumentation-native-windows-x86_64</artifactId>
        <version>1.0-SNAPSHOT</version>
        <scope>runtime</scope>
    </dependency>
    <dependency>
        <groupId>me.fan87</groupId>
        <artifactId>native-instrumentation-native-osx-aarch64</artifactId>
        <version>1.0-SNAPSHOT</version>
        <scope>runtime</scope>
    </dependency>
</dependencies>
```

#### About the Code Quality
All the C++ code in this repository are not modern C++, they are
written in a C-like way, and they are extremely old, since they were used
for an internal project that was active around 3 ~ 4 years ago, and I'm just
now open-sourcing this piece of the code.

*TL;DR, the code quality of C++ here is terrible, and I know that*

<p align="right"><sub><sup>Open Source Project</sup></sub></p>