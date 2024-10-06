from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import load, copy
from conan.tools.scm import Git
import re
import os

class HoriApiConan(ConanFile):
    name = "horiapi"
    description = "Hori API"
    license = "MIT"
    author = "Boguslaw Rymut (boguslaw@rymut.org)"
    topics = ("manager", "hori", "hid")
    generators = "CMakeDeps"

    settings = "os", "arch", "compiler", "build_type"
    requires = [
        "hidapi/0.14.0",
        "jansson/2.14",
        "argtable3/3.2.2",
        "libyaml/0.2.5"
    ]
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    def set_version(self):
        git = Git(self, self.recipe_folder)
        try:
            if git.is_dirty():
                self.version = "cci_%s" % datetime.datetime.utcnow().strftime('%Y%m%dT%H%M%S')
        except:
            pass
        try:
            tag = git.run("describe --tags").strip()
            if tag.startswith("v"):
                tag = tag[1:].strip()
            if re.match("^(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)(?:-(?P<prerelease>(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+(?P<buildmetadata>[0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$", tag):
                self.version = tag
                return
        except:
            pass
        try:
            self.version = load(self, "version.semver").strip()
            return
        except:
            pass
        try:
            self.version = "rev_%s" % git.get_commit().strip()
            return
        except:
            pass
        return None

    def requirements(self):
        self.test_requires("cmocka/1.1.7")

    def configure(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            test_folder = os.path.join("tests")
            if self.settings.os == "Windows":
                test_folder = os.path.join("tests", str(self.settings.build_type))
            self.run(os.path.join(test_folder, "test_hello"))
