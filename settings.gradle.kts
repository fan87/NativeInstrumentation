rootProject.name = "native-instrumentation"

buildscript {
    repositories {
        mavenCentral()
    }

    dependencies {
        classpath("kr.motd.maven:os-maven-plugin:1.7.1") {
            exclude("org.apache.maven:maven-plugin-api")
            exclude("org.codehaus.plexus:plexus-utils")
        }
    }
}

include("native-instrumentation-native")
