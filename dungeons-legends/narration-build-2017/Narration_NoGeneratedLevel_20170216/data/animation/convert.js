function traverse(res, node) {
	var children = node.tree || node.children;
	for (var i = 0; i < children.length; i++) {
		var obj = children[i];
		var target = {};

		target.name = obj.name.toLowerCase();

		obj.worldOffset = [
			obj.position[0] + obj.offsetFromPivot[0] + node.worldOffset[0], 
			obj.position[1] + obj.offsetFromPivot[1] + node.worldOffset[1], 
			-(obj.position[2] + obj.offsetFromPivot[2]) + node.worldOffset[2]
		];

		// target.reset = ?
		// target.inflate = ?
		// target.mirror = ?
		// target.neverRender = ?

		target.pivot = [
			obj.position[0] + node.worldOffset[0], 
			obj.position[1] + node.worldOffset[1], 
			-(obj.position[2] + node.worldOffset[2])
		];
		target.rotation = [
			obj.rotation[0], 
			-obj.rotation[1], 
			-obj.rotation[2]
		];
		//target.rotation = obj.rotation;

		if (obj.size[0] !== 0 || obj.size[1] !== 0 || obj.size[2] !== 0) {
			target.cubes = [{
				// target.inflate = ?
				"origin": [
					obj.offsetFromPivot[0] - obj.size[0] / 2, 
					obj.offsetFromPivot[1] - obj.size[1] / 2, 
					-obj.offsetFromPivot[2] - obj.size[2] / 2
				],
				"size": obj.size,
				"uv": [
					Math.max(obj.texOffset[0], 0), // make these wrap around texturewidth/height
					Math.max(obj.texOffset[1], 0)
				]
			}];
		}

		if (node.name) {
			target.parent = node.name.toLowerCase();		
		}

		target.material = "alpha";

		res.push(target);

		if (obj.children) {
			traverse(res, obj);
		}
	}
}

function transform(modelJson, data) {
	var boneArray = [];

	modelJson.worldOffset = [0, 0, 0];
	modelJson.position = [0, 0, 0];
	traverse(boneArray, modelJson);

	data["geometry.new." + modelJson.title.toLowerCase()] = {
		"texturewidth": modelJson.texturewidth || 64,
		"textureheight": modelJson.textureheight || 64,
		"newformat": true,
		"bones": boneArray
	};
}

function transformFile(inputFileName, transformedData) {
	try {
		var data = fs.readFileSync(inputFileName, "utf8");

		console.log("Converting file: " + inputFileName);

		var modelJson = JSON.parse(data);

		// Convert craftstudio json to minecraft json
		transform(modelJson, transformedData);
	} catch (err) {
		console.log(err);
	}
}

function saveFile(outputFileName, data) {
	fs.writeFile(outputFileName, JSON.stringify(data, null, '  '), function(err) {
		if (err) {
			return console.log(err);
		}
		console.log("Done!");
	});
}

var fs = require("fs");

function parseFiles(dir, transformedData) {
    var files = fs.readdirSync(dir);
    for (var i in files){
        var name = dir + '/' + files[i];
        if (fs.statSync(name).isDirectory()){
            parseFiles(name, transformedData);
        } else if (name.indexOf('.csjsmodel') === name.length - 10) {
        	transformFile(name, transformedData);
        }
    }
}

var transformedData = {};
parseFiles('.', transformedData);
saveFile('new_characters.json', transformedData);

