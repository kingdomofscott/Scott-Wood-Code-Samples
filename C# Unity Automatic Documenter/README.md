# C# Unity Automatic Documenter
This is an example of a tool I had written in and for Unity. Though, this is less of a "sample" and is instead the entire tool itself as it was pretty well encapsulated and presentable.
The intended purpose was to automate our need for easy to access and documentation of our entire project.
However, the issue with this tool is it was too bulky and didn't scale well as the project grows. The viewer was also not fully thought through.

The cool thing with this tool is it has the ability to:
* Read and document **all** assets inside the code
* Map **all** dependencies inside the project. This was especially useful in being specific and informed on what was being built into Asset Bundles

<img src="/Images/Documenter.png" width="300">
<img src="/Images/DocumenterOutput.png" width="490">

### Thoughts
A better implementation would be to:
* Write all of the documentation data into HTML through a Jenkins job
* Instead of using the built in Unity systems, read the meta files for each asset and parse the YAML to actually find what GUIDs are being used
* View it as a webpage complete with hyperlinks.

## Explanation

The tool makes use of Unity's built in systems for Asset handling. By reading the GUID of an Asset, we make a 'Dictionary' of GUIDs to Asset Names. Then grab all the dependencies by using the Unity `AssetDatabase.GetDependencies` function, and write that to the Asset Reference object. After all that, the data generated is written out to a file, and then the viewer is used to actually view all of the information.