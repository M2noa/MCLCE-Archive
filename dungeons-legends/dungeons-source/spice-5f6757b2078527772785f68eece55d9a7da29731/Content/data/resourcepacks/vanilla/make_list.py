import os
for root, dirs, files in os.walk("images/blocks"):
    for file in files:
        if file.endswith(".png") or file.endswith(".tga"):
        	path = os.path.join(root, file).replace("\\", "/")

        	key =  "block." + path[path.rfind("/") + 1:-4].replace("_", ".")

        	print('"' + key + '" : "' + path + '",')

for root, dirs, files in os.walk("images/items"):
    for file in files:
        if file.endswith(".png") or file.endswith(".tga"):
        	path = os.path.join(root, file).replace("\\", "/")

        	key =  "item." + path[path.rfind("/") + 1:-4].replace("_", ".")

        	print('"' + key + '" : "' + path + '",')