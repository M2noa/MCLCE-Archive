// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System.IO;

using UnrealBuildTool;

public class Dungeons : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string DependencyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../SourceDeps/")); }
    }

    private string BuildPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Build/")); }
    }

    public Dungeons(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
        PublicDependencyModuleNames.AddRange(new string[] { "ShaderCore", "RenderCore", "RHI", "RuntimeMeshComponent", "ImageWrapper" });

        LoadDependencies(Target);
    }

    public bool LoadDependencies(TargetInfo Target)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";

            PublicAdditionalLibraries.Add(Path.Combine(BuildPath, "Core", PlatformString, "Core.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(BuildPath, "json", PlatformString, "json.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(BuildPath, "Zlib", PlatformString, "Zlib.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(BuildPath, "Raknet", PlatformString, "Raknet.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(BuildPath, "LevelDB", PlatformString, "leveldb.lib"));

            Definitions.Add("_CRT_SECURE_NO_WARNINGS");
            Definitions.Add("OS_WIN");
//             Definitions.Add("ssize_t = signed long");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            // Do mac stuff
        }

        if (isLibrarySupported)
        {
            //PublicIncludePaths.Add(DependencyPath);
            PublicIncludePaths.Add(Path.Combine(DependencyPath, "Core"));
            PublicIncludePaths.Add(Path.Combine(DependencyPath, "json"));
            PublicIncludePaths.Add(Path.Combine(DependencyPath, "zlib"));
            PublicIncludePaths.Add(Path.Combine(DependencyPath, "raknet"));
//             PublicIncludePaths.Add(Path.Combine(DependencyPath, "leveldb"));
            PublicIncludePaths.Add(Path.Combine(DependencyPath, "leveldb/include"));
            PrivateIncludePaths.Add(Path.Combine(DependencyPath, "leveldb"));
        }

        Definitions.Add(string.Format("WITH_DEPENDENCIES_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
