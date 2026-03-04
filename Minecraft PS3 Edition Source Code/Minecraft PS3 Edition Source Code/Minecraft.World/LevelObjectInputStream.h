#pragma once

class LevelObjectInputStream;

/*

4J This code is not used.

class LevelObjectInputStream : ObjectInputStream
{
    private Set<String> autoReplacers = new HashSet<String>();

    public LevelObjectInputStream(InputStream in) throws IOException
    {
        super(in);

        autoReplacers.add("com.mojang.minecraft.player.Player$1");
        autoReplacers.add("com.mojang.minecraft.mob.Creeper$1");
        autoReplacers.add("com.mojang.minecraft.mob.Skeleton$1");
    }

protected:
	ObjectStreamClass readClassDescriptor() // 4J - throws IOException, ClassNotFoundException
    {
        ObjectStreamClass osc = super.readClassDescriptor();

        if (autoReplacers.contains(osc.getName()))
        {
            return ObjectStreamClass.lookup(Class.forName(osc.getName()));
        }
        return osc;
    }
}
*/