using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System;
using System.Reflection;
using System.IO;

namespace Scott.Documentor
{
    #region Documentor
    public class AutomatedDocumenter : EditorWindow
    {
        #region Properties
        private Dictionary<string, AssetReferenceContainer> _assetReferences;
        private Dictionary<string, List<string>> _assetDependencies;
        private string _savePath;
        private string _searchPath;
        private bool _searchPrefabs = true;
        private bool _searchScriptables = true;
        private bool _searchTextures = true;
        private bool _searchScripts = true;
        private bool _searchAnimations = true;
        private bool _searchAudioClips = true;
        private bool _searchScenes = true;
        private bool _searchMaterials = true;
        private string _extraExtensions = "";
        private bool _openViewer = true;
        private string _status = "Not Started";

        private List<string> dependencyList;
        #endregion

        [MenuItem("Scott/Tools/Generate Documentation")]
        public static void ShowWindow()
        {
            AutomatedDocumenter window = (AutomatedDocumenter)EditorWindow.GetWindow(typeof(AutomatedDocumenter));

            window.Repaint();
        }

        public void OnGUI()
        {
            GUILayout.Label("_status: " + _status);

            GUILayout.FlexibleSpace();

            _searchPrefabs = EditorGUILayout.Toggle("Document Prefabs", _searchPrefabs);
            _searchScriptables = EditorGUILayout.Toggle("Document Scriptables", _searchScriptables);
            _searchTextures = EditorGUILayout.Toggle("Document Textures", _searchTextures);
            _searchScripts = EditorGUILayout.Toggle("Document Scripts", _searchScripts);
            _searchAnimations = EditorGUILayout.Toggle("Document Animations", _searchAnimations);
            _searchAudioClips = EditorGUILayout.Toggle("Document Audio Clips", _searchAudioClips);
            _searchScenes = EditorGUILayout.Toggle("Document Scenes", _searchScenes);
            _searchMaterials = EditorGUILayout.Toggle("Document Materials", _searchMaterials);

            _extraExtensions = EditorGUILayout.TextField("Extra file extensions to check (; delimted): ", _extraExtensions);

            GUILayout.FlexibleSpace();

            #region File Pathing
            EditorGUILayout.LabelField("*This is relative to the Assets folder");
            _searchPath = EditorGUILayout.TextField("Search Path", _searchPath);

            _savePath = EditorGUILayout.TextField("Save Path", _savePath);

            if (string.IsNullOrEmpty(_savePath))
            {
                GUI.backgroundColor = Color.red;
            }
            else
            {
                GUI.backgroundColor = Color.green;
            }

            if (GUILayout.Button("Browse Save Path"))
            {
                _savePath = EditorUtility.SaveFilePanel("Save As...", Application.dataPath, "Documentation.txt", "txt");
            }
            #endregion

            GUILayout.FlexibleSpace();

            GUI.backgroundColor = Color.white;
            _openViewer = EditorGUILayout.Toggle("Open Viewer", _openViewer);
            EditorGUILayout.BeginHorizontal();

            bool ableToStart = !string.IsNullOrEmpty(_savePath);

            if (ableToStart)
            {
                GUI.backgroundColor = Color.green;
            }
            else
            {
                GUI.backgroundColor = Color.gray;
            }

            if (GUILayout.Button("Document") && ableToStart)
            {
                _status = "Working...";
                StartDocumenting();
            }
        }

        #region Thread handling
        public void StartDocumenting()
        {
            //We make two different lists because multiple passes need to be made for building a list first and then filling dependencies
            _assetReferences = new Dictionary<string, AssetReferenceContainer>();
            _assetDependencies = new Dictionary<string, List<string>>();

            MakeReferenceDictionary();
            SaveFile();

            _status = "Finished!";

            if (_openViewer)
            {
                DocumentorViewer._start_loadPath = _savePath;
                DocumentorViewer.ShowWindow();
                Close();
            }
        }
        #endregion

        #region Logic
        //Build a dictionary of assets and what uses them
        //Each GUID in _assetReferences contains data about the asset and guids of assets referencing this asset
        private void MakeReferenceDictionary()
        {
            string[] searchInFolders = { Get_searchPathModified() };

            List<string> guids = new List<string>(AssetDatabase.FindAssets(
                GetTypesToSearch(),
                searchInFolders));

            //Search non unity assets
            if (!string.IsNullOrEmpty(_extraExtensions))
            {
                string[] allAssets = AssetDatabase.GetAllAssetPaths();
                string[] acceptableExtensions = _extraExtensions.Split(';');

                for (int i = 0; i < allAssets.Length; ++i)
                {
                    for (int j = 0; j < acceptableExtensions.Length; ++j)
                    {
                        if (Path.GetExtension(allAssets[i]).Equals(acceptableExtensions[j]))
                        {
                            guids.Add(AssetDatabase.AssetPathToGUID(allAssets[i]));
                            break;
                        }
                    }
                }
            }

            string[] assetSections, dependencies;
            string assetPath;
            UnityEngine.Object obj;
            AssetReferenceContainer arc;
            AssetReference assetRef;

            for (int i = 0; i < guids.Count; ++i)
            {
                if (_assetReferences.ContainsKey(guids[i]))//This can happen with animation controllers containing anims
                    continue;

                assetPath = AssetDatabase.GUIDToAssetPath(guids[i]);

                arc = new AssetReferenceContainer();
                assetSections = assetPath.Split('.');

                arc.guid = guids[i];
                arc.assetType = assetSections[assetSections.Length - 1];
                assetSections = assetSections[assetSections.Length - 2].Split('/');
                arc.fileName = assetSections[assetSections.Length - 1];
                arc.assetBundleName = AssetDatabase.GetImplicitAssetBundleName(assetPath);

                obj = AssetDatabase.LoadAssetAtPath(assetPath, typeof(UnityEngine.Object));

                if (obj is MonoScript)//Fill out script components
                    WriteOutScriptProperties((obj as MonoScript), ref arc);

                _assetReferences.Add(guids[i], arc);

                //Grab dependenices here to save another pass
                dependencies = AssetDatabase.GetDependencies(assetPath);
                dependencyList = new List<string>();
                for (int d = 0; d < dependencies.Length; ++d)
                    dependencyList.Add(AssetDatabase.AssetPathToGUID(dependencies[d]));
                _assetDependencies.Add(guids[i], dependencyList);
            }

            //Now loop through the dependencies and add the references to _assetReferences
            //Add the key as a ref to all of its dependencies
            foreach (KeyValuePair<string, List<string>> assetDependency in _assetDependencies)
            {
                assetRef = new AssetReference();
                assetRef.fileName = _assetReferences[assetDependency.Key].fileName;
                assetRef.guid = assetDependency.Key;
                assetRef.assetBundle = _assetReferences[assetDependency.Key].assetBundleName;

                for (int i = 0; i < assetDependency.Value.Count; ++i)
                {
                    //Checking if we documented the referenced asset and it is not self dependent
                    if (_assetReferences.ContainsKey(assetDependency.Value[i]) && assetDependency.Value[i] != assetDependency.Key)
                        _assetReferences[assetDependency.Value[i]].references.Add(assetRef);
                }
            }
        }
        #endregion

        #region Helper Methods
        private string Get_searchPathModified()
        {
            if (string.IsNullOrEmpty(_searchPath))
                return "Assets";

            return "Assets/" + _searchPath;
        }

        private string GetTypesToSearch()
        {
            string types = "";

            if (_searchPrefabs)
                types += "t: prefab ";
            if (_searchScriptables)
                types += "t: scriptableobject ";
            if (_searchTextures)
                types += "t: texture ";
            if (_searchScripts)
                types += "t: script ";
            if (_searchAnimations)
                types += "t: animationclip t: animatorcontroller ";
            if (_searchAudioClips)
                types += "t: audioclip ";
            if (_searchScenes)
                types += "t: scene ";
            if (_searchMaterials)
                types += "t: material ";


            return types;
        }

        private void WriteOutScriptProperties(MonoScript script, ref AssetReferenceContainer refContainer)
        {
            Type classType = script.GetClass();
            if (classType == null)//couldn't get a class
                return;

            MethodInfo[] methods = classType.GetMethods();

            ParameterInfo[] parameters;
            MemberInfo[] members = classType.GetMembers();
            ScriptMethod newMethod;

            foreach (MethodInfo method in methods)
            {
                if (!method.IsPublic || method.DeclaringType != classType)//We only care for public functions made in this script
                    continue;

                //Add method name and return type
                newMethod = new ScriptMethod();
                newMethod.name = method.Name;

                newMethod.returnType = GetStrippedType(method.ReturnType.ToString());
                parameters = method.GetParameters();

                //Add method parameters
                foreach (ParameterInfo parameter in parameters)
                {
                    newMethod.parameters.Add(GetStrippedType(parameter.ParameterType.ToString()) + " " + parameter.Name);
                }

                refContainer.methods.Add(newMethod);
            }

            foreach (MemberInfo member in members)
            {
                if (member.DeclaringType != classType)
                    continue;

                if (member.MemberType == MemberTypes.Property || member.MemberType == MemberTypes.Field)
                {
                    refContainer.members.Add(GetStrippedType(member.ToString()));
                }
            }
        }

        private string GetStrippedType(string typeString)
        {
            string[] typeSections = typeString.Split('.');
            if (typeSections.Length > 0)
                return typeSections[typeSections.Length - 1];
            else
                return typeString;

        }

        private void SaveFile()
        {
            List<AssetReferenceContainer> refs = new List<AssetReferenceContainer>();

            foreach (KeyValuePair<string, AssetReferenceContainer> obj in _assetReferences)
            {
                refs.Add(obj.Value);
            }

            string[] saveString = { JsonHelper.ToJson<AssetReferenceContainer>(refs.ToArray()) };

            System.IO.File.WriteAllLines(_savePath, saveString);
        }
        #endregion
    }
    #endregion

    #region Viewer
    public class DocumentorViewer : EditorWindow
    {
        #region Properties
        public static string _start_loadPath;
        private string _loadPath;
        private string _searchEntry;

        private bool _validFileLoaded = false;
        private Dictionary<string, AssetReferenceContainer> _loadedRefs;
        private Dictionary<string, bool> _loadedRefsShown;
        private List<string> _searchRefsKeys;
        private Vector2 _scrollPos;
        private string _prevSearchString;
        #endregion

        [MenuItem("Scott/Tools/View Documentation")]
        public static void ShowWindow()
        {
            DocumentorViewer window = (DocumentorViewer)EditorWindow.GetWindow(typeof(DocumentorViewer));

            window.Repaint();
        }

        public void OnGUI()
        {
            #region File Pathing
            bool _loadPathSentIn = !string.IsNullOrEmpty(_start_loadPath);

            if (_loadPathSentIn)
            {
                _loadPath = _start_loadPath;
                _start_loadPath = "";
                ReadFile();
            }

            _loadPath = EditorGUILayout.TextField("Read Path", _loadPath);

            if (!_validFileLoaded)
                GUI.backgroundColor = Color.red;
            else
                GUI.backgroundColor = Color.green;

            if (GUILayout.Button("Load Documentation"))
            {
                _loadPath = EditorUtility.OpenFilePanelWithFilters("What to read?", Application.dataPath, new string[] { "Text files", "txt" });
                ReadFile();
            }

            GUI.backgroundColor = Color.white;
            #endregion

            if (!_validFileLoaded)
                return;

            #region Display
            EditorGUI.BeginChangeCheck();

            _searchEntry = EditorGUILayout.TextField("Search: ", _searchEntry);

            if (EditorGUI.EndChangeCheck())
                Search();

            Separator(Color.gray);

            GUILayout.Space(10.0f);

            _scrollPos = EditorGUILayout.BeginScrollView(_scrollPos, false, true);

            if (_searchRefsKeys != null)
            {
                for (int i = 0; i < _searchRefsKeys.Count; ++i)
                {
                    if (_loadedRefsShown[_searchRefsKeys[i]] = EditorGUILayout.Foldout(_loadedRefsShown[_searchRefsKeys[i]],
                        _loadedRefs[_searchRefsKeys[i]].fileName + " - " + _loadedRefs[_searchRefsKeys[i]].assetBundleName + " (" + _searchRefsKeys[i] + ")"))
                    {
                        DrawAssetDetails(_loadedRefs[_searchRefsKeys[i]]);
                    }
                }
            }
            else if (!string.IsNullOrEmpty(_searchEntry))
            {
                EditorGUILayout.LabelField("No results found");
            }
            else//Show all
            {
                foreach (KeyValuePair<string, AssetReferenceContainer> valuePair in _loadedRefs)
                {
                    if (_loadedRefsShown[valuePair.Key] = EditorGUILayout.Foldout(_loadedRefsShown[valuePair.Key], valuePair.Value.fileName + " - " + valuePair.Value.assetBundleName
                        + " (" + valuePair.Value.guid + ")"))
                    {
                        DrawAssetDetails(valuePair.Value);
                    }
                }
            }

            EditorGUILayout.EndScrollView();
            #endregion

            Search();
        }

        private void ReadFile()
        {
            if (string.IsNullOrEmpty(_loadPath))
                return;

            string[] file = System.IO.File.ReadAllLines(_loadPath);

            if (file.Length == 0)
                return;

            AssetReferenceContainer[] assRefs = JsonHelper.FromJson<AssetReferenceContainer>(file[0]);
            _loadedRefs = new Dictionary<string, AssetReferenceContainer>();
            _loadedRefsShown = new Dictionary<string, bool>();

            for (int i = 0; i < assRefs.Length; ++i)
            {
                _loadedRefs[assRefs[i].guid] = assRefs[i];
                _loadedRefsShown[assRefs[i].guid] = false;
            }

            _validFileLoaded = true;
        }

        private void Search()
        {
            if (string.IsNullOrEmpty(_searchEntry))
            {
                _searchRefsKeys = null;
                _prevSearchString = "";
                return;
            }

            if (!string.IsNullOrEmpty(_prevSearchString) && _searchEntry.Length == _prevSearchString.Length)
                return;

            if (_searchRefsKeys == null)//Was empty search
                SearchAllReferences();
            else if (_searchEntry.Length > _prevSearchString.Length)//Added to previous search (refine current results)
                RefineSearch();
            else//removed from prev search, need to use all results
                SearchAllReferences();

            _prevSearchString = _searchEntry;
        }

        private void SearchAllReferences()
        {
            _searchRefsKeys = new List<string>();
            string lowerSearch = _searchEntry.ToLower();

            foreach (KeyValuePair<string, AssetReferenceContainer> valuePair in _loadedRefs)
            {
                if (valuePair.Value.fileName.ToLower().Contains(lowerSearch) ||
                    valuePair.Key.ToLower().Contains(lowerSearch))
                {
                    _searchRefsKeys.Add(valuePair.Key);
                }
                else if (valuePair.Value.references.Count > 0)
                {
                    //Check if the references contain the search
                    for (int i = 0; i < valuePair.Value.references.Count; ++i)
                    {
                        if (valuePair.Value.references[i].fileName.ToLower().Contains(lowerSearch) ||
                    valuePair.Value.references[i].guid.ToLower().Contains(lowerSearch))
                        {
                            _searchRefsKeys.Add(valuePair.Key);
                        }
                    }
                }
            }

            if (_searchRefsKeys.Count <= 0)
                _searchRefsKeys = null;
        }

        private void RefineSearch()
        {
            List<string> removedKeys = new List<string>();
            string lowerSearch = _searchEntry.ToLower();

            for (int i = 0; i < _searchRefsKeys.Count; ++i)
            {
                if (!_loadedRefs[_searchRefsKeys[i]].fileName.ToLower().Contains(lowerSearch) &&
                    !_searchRefsKeys[i].ToLower().Contains(lowerSearch))
                {
                    if (_loadedRefs[_searchRefsKeys[i]].references.Count <= 0)
                    {
                        removedKeys.Add(_searchRefsKeys[i]);
                        continue;
                    }

                    //Check the references to be sure
                    for (int j = 0; j < _loadedRefs[_searchRefsKeys[i]].references.Count; ++j)
                    {
                        if (!_loadedRefs[_searchRefsKeys[i]].references[j].fileName.ToLower().Contains(lowerSearch) &&
                            !_loadedRefs[_searchRefsKeys[i]].references[j].guid.ToLower().Contains(lowerSearch))
                        {
                            //If the entry isn't in anything
                            removedKeys.Add(_searchRefsKeys[i]);
                            continue;
                        }
                    }
                }
            }

            for (int r = 0; r < removedKeys.Count; ++r)
            {
                _searchRefsKeys.Remove(removedKeys[r]);
            }

            if (_searchRefsKeys.Count <= 0)
                _searchRefsKeys = null;
        }

        private void DrawAssetDetails(AssetReferenceContainer ass)
        {
            ++EditorGUI.indentLevel;

            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Type: ");
            EditorGUILayout.LabelField(ass.assetType, EditorStyles.boldLabel);
            EditorGUILayout.EndHorizontal();

            //References
            ++EditorGUI.indentLevel;
            if (ass.references.Count > 0)
            {
                EditorGUILayout.LabelField("Referenced by: ");

                ++EditorGUI.indentLevel;
                for (int i = 0; i < ass.references.Count; ++i)
                {
                    if (_loadedRefs.ContainsKey(ass.references[i].guid))
                    {
                        EditorGUILayout.LabelField(ass.references[i].fileName + " - " + ass.references[i].assetBundle + " (" + ass.references[i].guid + ")");
                    }
                }
                --EditorGUI.indentLevel;
            }
            else
            {
                EditorGUILayout.LabelField("Not referenced by anything");
            }
            --EditorGUI.indentLevel;

            //Methods
            ++EditorGUI.indentLevel;
            if (ass.methods.Count > 0)
            {
                EditorGUILayout.LabelField("Public Methods: ");

                string parameters = "";
                ++EditorGUI.indentLevel;
                for (int i = 0; i < ass.methods.Count; ++i)
                {
                    parameters = "";
                    for (int j = 0; j < ass.methods[i].parameters.Count; ++j)
                    {
                        parameters += ass.methods[i].parameters[j];
                        if (j != ass.methods[i].parameters.Count - 1)
                            parameters += ", ";
                    }

                    EditorGUILayout.LabelField(ass.methods[i].returnType + " " + ass.methods[i].name + "(" + parameters + ")");
                }
                --EditorGUI.indentLevel;
            }
            --EditorGUI.indentLevel;

            //Members
            ++EditorGUI.indentLevel;
            if (ass.members.Count > 0)
            {
                EditorGUILayout.LabelField("Public Variables: ");

                ++EditorGUI.indentLevel;
                for (int i = 0; i < ass.members.Count; ++i)
                {
                    EditorGUILayout.LabelField(ass.members[i]);
                }
                --EditorGUI.indentLevel;
            }
            --EditorGUI.indentLevel;

            --EditorGUI.indentLevel;
        }

        private void Separator(Color color, int thickness = 1, int padding = 10, int margin = 5)
        {
            Rect rect = EditorGUILayout.GetControlRect(GUILayout.Height(thickness + padding));
            rect.x = margin;
            rect.y += padding / 2;
            rect.width -= margin / 2;
            rect.height = thickness;

            EditorGUI.DrawRect(rect, color);
        }
    }
    #endregion

    [Serializable]
    public class AssetReferenceContainer
    {
        public string guid = "";
        public string fileName = "";
        public string assetType = "";
        public string assetBundleName = "";
        public List<AssetReference> references = new List<AssetReference>();//GUIDs of the assets that reference this object
        public List<string> comments = new List<string>();
        public List<ScriptMethod> methods = new List<ScriptMethod>();
        public List<string> members = new List<string>();
    }

    [Serializable]
    public class AssetReference
    {
        public string guid;
        public string fileName;
        public string assetBundle;
    }

    [Serializable]
    public class ScriptMethod
    {
        public string name;
        public string returnType;
        public List<string> parameters = new List<string>();
    }

    #region JsonHelper
    public static class JsonHelper
    {
        public static T[] FromJson<T>(string json)
        {
            Wrapper<T> wrapper = JsonUtility.FromJson<Wrapper<T>>(json);
            return wrapper.Items;
        }

        public static string ToJson<T>(T[] array)
        {
            Wrapper<T> wrapper = new Wrapper<T>();
            wrapper.Items = array;
            return JsonUtility.ToJson(wrapper);
        }

        public static string ToJson<T>(T[] array, bool prettyPrint)
        {
            Wrapper<T> wrapper = new Wrapper<T>();
            wrapper.Items = array;
            return JsonUtility.ToJson(wrapper, prettyPrint);
        }

        [Serializable]
        private class Wrapper<T>
        {
            public T[] Items;
        }
    }
    #endregion
}
