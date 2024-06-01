import java.io.File

plugins {
    id("java")
    id("maven-publish")
}

group = "me.fan87"
version = "1.0-SNAPSHOT"

fun isWindows(): Boolean = "Windows" in System.getProperty("os.name")

repositories {
    mavenCentral()
}

dependencies {
    testImplementation("org.junit.jupiter:junit-jupiter:5.7.1")
    testRuntimeOnly(project(":native-instrumentation-native"))

    testImplementation("org.ow2.asm:asm:9.3")
    testImplementation("org.ow2.asm:asm-util:9.3")
    testImplementation("org.ow2.asm:asm-commons:9.3")
}

tasks.compileJava {
    options.headerOutputDirectory.set(File(project(":native-instrumentation-native").projectDir, "javah"))
}

tasks.test {
    useJUnitPlatform()
}

val cmakeTargets = listOf("native")
val cmakeBuildDir = layout.buildDirectory.dir("cmake").get()
cmakeBuildDir.asFile.mkdirs()

extensions.add("cmakeBuildDir", cmakeBuildDir)
private fun getOutputFile(target: String): RegularFile {
    return cmakeBuildDir.dir("lib").file(System.mapLibraryName(target))
}

val setupCmake by tasks.register<Exec>("setupCmake") {
    group = "cmake"
    commandLine("cmake", "-DCMAKE_BUILD_TYPE=Debug", "-Dinject_method=ptrace", projectDir.absolutePath)
    workingDir(cmakeBuildDir)
}

publishing {
    publications {
        register<MavenPublication>("mavenJava") {
            from(components.getByName("java"))
        }
    }
}

allprojects {
    version = rootProject.version
    group = rootProject.group
    if (tasks.findByName("compileJava") != null) {
        tasks.compileJava {
            options.encoding = "utf-8"
        }
    }
    if (extensions.findByName("publishing") != null) {
        publishing {
            publications.configureEach {
                if (this is MavenPublication) {
                    pom {
                        url.set("https://github.com/fan87/NativeInstrumentation")
                        scm {
                            url.set("https://github.com/fan87/NativeInstrumentation")
                        }
                        licenses {
                            license {
                                name.set("GNU General Public License v3.0")
                                url.set("https://www.gnu.org/licenses/gpl-3.0.txt")
                                distribution.set("repo")
                            }
                        }
                    }
                }
            }
        }
    }
}