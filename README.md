A wind sound generator plugin with intensity, amplitude, resonance, speed, pan, whistle and howl controls.

<img width="223" alt="Screenshot 2023-12-17 182324" src="https://github.com/ashaydave/WindControl/assets/112194962/3aa30614-5a67-42c7-b8b1-663ad129bcfa">

[WINDOWS] Just drag and drop the .dll file into Unity > Assets > Plugins folder in your Unity project, then add the effect on your desired Audio Mixer track.

If you'd like to build the solution yourself:

1. Clone or download the repo.
2. Open the Projucer File (Download and install [JUCE](https://github.com/juce-framework/JUCE)).
3. Build the Solution
4. After building, navigate to the extracted folder > Builds > VisualStudio or your IDE > x64 > Release > Unity Plugin.
5. Plugin will be named as audioplugin_WindControl.dll.
6. Drag the audioplugin_WindControl.dll into Unity > Assets > Plugins folder.

[MacOS] After building in Xcode, drag and drop the .bundle folder into your Plugins folder.
