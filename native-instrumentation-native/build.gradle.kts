import kr.motd.maven.os.Detector
import org.gradle.api.Plugin
import org.gradle.api.Project
import org.gradle.api.attributes.LibraryElements
import org.gradle.api.component.SoftwareComponentFactory
import org.gradle.api.file.Directory
import org.gradle.api.tasks.Exec
import org.gradle.api.tasks.bundling.Jar
import org.gradle.kotlin.dsl.*
import org.slf4j.LoggerFactory
import java.util.*
import javax.inject.Inject

plugins {
    id("maven-publish")
}

class CMakePlugin @Inject constructor(
    private val softwareComponentFactory: SoftwareComponentFactory
) : Plugin<Project> {
    private val logger = LoggerFactory.getLogger(Detector::class.java)
    override fun apply(project: Project) {
        val setupCmake = project.rootProject.tasks.getByName("setupCmake") as Exec
        val cmakeBuildDir: Directory by project.rootProject.extensions
        val impl = DetectorImpl()
        val target = project.name.replace("-", "_")
        val cmakeBuild = project.tasks.register<Exec>("cmakeBuild") {
            group = "cmake"
            dependsOn(setupCmake)
            outputs.upToDateWhen { false }
            commandLine("cmake", "--build", cmakeBuildDir.asFile.absolutePath, "--target", target)
            workingDir(cmakeBuildDir)
        }
        val jar = project.tasks.register<Jar>("jar") {
            dependsOn(cmakeBuild)
            from(cmakeBuildDir.dir("lib").file(System.mapLibraryName(target))) {
                rename { System.mapLibraryName(target + "-" + impl.properties[Detector.DETECTED_CLASSIFIER]) }
            }
            archiveBaseName.set(project.name)
            archiveVersion.set(project.version.toString())
            archiveClassifier.set(impl.properties[Detector.DETECTED_CLASSIFIER].toString())
            destinationDirectory.set(project.layout.buildDirectory.dir("libs"))
        }
        val outgoing by project.configurations.creating {
            isCanBeConsumed = true
            isCanBeResolved = false
            attributes {
                attribute(LibraryElements.LIBRARY_ELEMENTS_ATTRIBUTE, project.objects.named(LibraryElements.JAR))
            }
        }
        project.tasks.register("assemble") {
            group = "build"
            dependsOn(jar)
        }
        project.tasks.register("build") {
            group = "build"
            dependsOn("assemble")
        }
        val nativeComponent = softwareComponentFactory.adhoc("native")
        project.components.add(nativeComponent)
        nativeComponent.addVariantsFromConfiguration(outgoing) {
            mapToMavenScope("runtime")
        }
        project.artifacts.add("outgoing", jar)
    }

    private inner class DetectorImpl : Detector() {
        val properties = Properties()

        init {
            detect(properties, arrayListOf())
        }

        override fun log(message: String?) {
            logger.info(message)
        }

        override fun logProperty(name: String?, value: String?) {
        }

    }
}


apply<CMakePlugin>()

tasks.getByName("cmakeBuild").dependsOn(":compileJava") // Needs javah


publishing {
    publications {
        create<MavenPublication>("maven") {
            from(components["native"])
        }
    }
}