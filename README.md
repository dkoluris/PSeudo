<h1>PSeudo</h1>

[![GitHub stars](https://img.shields.io/github/stars/dkoluris/pseudo.svg?style=flat-square)](https://github.com/dkoluris/pseudo/stargazers) [![GitHub license](https://img.shields.io/github/license/dkoluris/pseudo.svg?style=flat-square)](https://github.com/dkoluris/pseudo/blob/master/LICENSE) [![Twitter](https://img.shields.io/twitter/url/https/github.com/dkoluris/pseudo.svg?style=social)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Fdkoluris%2Fpseudo)

**PSeudo aims to be the world's simplest PSX emulator** both in terms of user experience and software implementation. It is coded in **C/C++** and is utilizing modern APIs such as **OpenGL** for visuals and **OpenAL** for audio. I have been developing this emulator on a Mac, so for the time being it works on **macOS** & **iOS**. Some experiments are complete on **Windows** platform as well, but still needs work. In time, more platforms will be supported, as I am aiming for a cross-platform solution.

<a href="https://github.com/dkoluris/pseudo/releases/tag/version-0.8">>> Get the latest PSeudo 0.8 release for Mac <<</a>

<h2>Showcase</h2>

<a href="http://www.youtube.com/watch?v=BFvLRjOE99E">Check out the video below, for a homebrew demo/cracktro showcase:<br/>

<img alt="FF9" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/FF9.jpg" width="48.5%"/><img alt="Super Pox" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/Pox.jpg" width="48.5%" align="right"/>

<img alt="Casper" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/Casper.jpg" width="48.5%"/><img alt="Paradox Deadline" src="https://raw.githubusercontent.com/dkoluris/pseudo/master/Resources/Deadline.jpg" width="48.5%" align="right"/>

</a>

<h2>Completion</h2>

Here's a list with the overall progress of the emulator, broken down in distinct parts. Components with 0% progress are not listed.
* `95% -> CPU Mips R3000A`
* `90% -> DMA`
* `85% -> Mem IO`
* `85% -> Movie Decoder`
* `80% -> Interrupts`
* `75% -> CD Decoder`
* `70% -> GPU Primitives & Commands`
* `65% -> GPU Textures`
* `60% -> Audio`
* `60% -> GTE`
* `55% -> Rootcounters`
* `25% -> Serial IO`
* `10% -> XA Audio`

**PSeudo** can load some initial game screens, but nothing more for now. Also, for quite some time I will keep working on it with the provided slow CPU Interpreter.

<h2>Compile / Build</h2>

In order to build the application on **macOS** & **iOS**, the prerequisite is Xcode 9+. For **Windows** install **Microsoft Visual Studio 2015** and on top of that a prebuilt version of **LLVM 6.0**, we will use that instead of MSVC. This is important in order to keep the source code identical between different platforms.

<h2>License</h2>

Open-source under [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0).
