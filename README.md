# GamedevResourcePacker

Resource (assets) packaging software for gamedev projects. Can be
used for:

* Game objects XML verification.

* Game objects convertion from XML to binary representation (speeds up
   resource loading).

* `C++` code generation for game resources (loading
  from binary form implementation).

* Game resource compression (work in progress).

## Usage basics

### Call arguments

```
${executable} ${config dir} ${raw assets dir} ${assets output} ${code output}
```

#### What is config dir?

Contains plugin-specific configuration files, such as data classes for
`DataObjects` plugin.

#### What is raw assets dir?

It's a directory with assets that you want to pack with
GamedevResourcePacker. GamedevResourcePacker recursively scans all
files and assigns them to processing plugins. If file can not be
assigned to any processing plugin, warning will be shown.

#### What is assets output?

It's a directory where binary assets will be stored after generation.
By default binary output is organized as plain directory where each file
is a binary resource object.

#### What is code output?

It's a directory where resource subsystem code will be stored after
processing. It contains resource subsystem core code (which is used to manage loaded resource objects) and resource objects classes.

### What is resource object?

After scanning, all assigned files are parsed by their processing plugins as resource objects. During this process plugin assigns name and
resource class to new resource object. Each plugin has it's own name
and class assignment rules and you must ensure that any resource
object must have unique name hash in its resource class.

Resource object represents a single assets unit, that will be processed (and used in game) separately and can be referenced from other resource objects. It may be image, tile map or character, but it should not be something like tile (because tile is used only in tile maps) or bone (because bones are used only in skeletons).

### What is stored in content.list file?

`content.list` stores information (in binary form) from which file resource objects must be loaded. This is `content.list` format:

```
N - resource classes count (std::size_t)
N times:
    class name hash (uint32_t)
    M - class instance count (std::size_t)
    M - times:
        object name hash (uint32_t)
        K - object file path string length (uint32_t)
        object file path string (K bytes)
        object data offset in file (uint32_t)
```

You can figure, that this content list structure allows you to repack assets as you need without editing generated code.

### What is processing plugin?

It's a library that actually decides what assets to capture and how to check and pack them. For example, image plugin will capture all images and data object plugin will capture data objects (like tile map). From this point of view GamedevResourcePacker is an ecosystem that allows multiple plugins to work together.

# DataObjects plugin

## Description

DataObjects plugin works with dataobject-like assets, such as tile maps or unit types. You must define data object classes you need, then plugin will be able to parse and verify data objects in assets, pack them and generate C++ code for `ResourceSubsystem`. Also data objects support referencing, that means that you can specify tile set name in tile map, then this reference will be verified and resolved during packaging. After that, when game starts loading tile map, specified tile set will be loaded and referenced automatically as dependency.

### What is data class?

Data class is a class-like pattern, used to define information format. It's list of fields (like usual class) where each field can be single value, array, reference of array of references.

### What is data object?

Data object is a top level instance of data class. In other words, data object can contain other instances of different data classes as fields, but there is no such instance of any data class that contains data object as value. So data objects can only be referenced (like resource objects). From GamedevResourcePacker perspective data objects are usual resource objects, so they are parsed separately and can be referenced from any instance of any data class. Remember that only data objects can be referenced because GamedevResourcePacker supports only resource object to resource object references.

### They are very small, why we pack them?

Yes, data objects are really smaller than models or images. But, as you know, devil hides in details. In this case most problematic detail is text parsing -- xml or json parsing is much less efficient than binary data copying from drive to memory. You think this delay won't be a problem? Lets think about this two cases:

1. You're mobile developer. Casual player sessions are quite small (like 20 minutes) and first minutes of gameplay are usually the most influential, so even 30 additional seconds of loading can be a problem. From my experience, even in binary mode location loading in simple 3d farming game can take about 20-30 seconds. With XML location description it could take up to 90 seconds, which is terrible.

2. You're developing game with open world. During gameplay you must load new map chunks, but you must be very accurate with it. Surely, you don't want to decrease FPS by half or even freeze the game. So, it's time to think about text to binary conversion of your game chunks. UE4 and Unity already use this principle.

3. You just have a lot of text assets that you have to parse. For example think about Paradox strategies: lots of provinces and countries, their history, their parameters and lots of other configuration. Yes, Paradox uses text format for this data, but are you satisfied with how much time UE4 loads this information?

## Usage basics

To use DataObjects plugin you must do three things:

1. Add DataObjects plugin DLL to GamedevResourcePacker `plugins` directory. Which is already because it's a default plugin.

2. Define data classes.

3. Define your data objects as assets.

### Defining data object classes

Data classes are defined by `xml`s in `${config_dir}/DataObjectClasses` dir and its subdirs. Lets check example tile map data class definition:

```xml
<?xml version="1.0" encoding="utf-8" ?>
<class name="TileMap">
    <field name="mapWidth" typeName="int" array="false" reference="false" readonly="true"/>
    <field name="mapHeight" typeName="int" array="false" reference="false" readonly="true"/>
    <field name="tiles" typeName="Tile" array="true" reference="false" readonly="true"/>
    <field name="objects" typeName="MapObject" array="true" reference="false" readonly="true"/>
    <field name="tileSet" typeName="TileSet" array="false" reference="true" readonly="true"/>
    <field name="objectSet" typeName="MapObjectSet" array="false" reference="true" readonly="true"/>
</class>
```

XML root element must be named `class` and must have `name` attribute with value that can be used in C++ as class name. `class` element must have only `field` children, so lets talk about fields attributes:

1. `name` -- must be specified. Name of this field, must be correct as xml element and C++ variable name.

2. `typeName`  -- must be specified. Can be `int`, `float`, `string` or name of resource class. Each data class is a resource class too. If it's an `array` field, it will be array of instances of this type instead of single value. If it's a `reference` field, it will store reference to resource object of given resource type. Reference fields can not have `int`, `float` or `string` resource types. Finally, if it's both `reference` and `array` field, it will be an array of references.

3. `array` -- must be specified. Is it an `array` field?

4. `reference` -- must be specified. Is it a `reference` field?

5. `readonly` -- must be specified. C++ setters won't be generated for `readonly` fields.

### Defining data objects

DataObjects plugin checks every xml in assets directory and if it's has `data-object` root element, captures it as data object. Let's check this little data object definition:

```xml
<?xml version="1.0" encoding="utf-8" ?>
<data-object name="Default">
    <TileSet atlasImage="Images/DefaultAtlas.png" rows="4" cols="2"/>
</data-object>
```

Root `data-object` element must have `name` attribute that specifies data object name. This name must be unique in resource class. `data-object` element must have one child which name must be data class name of this data object. In this example it's `TileSet`.

#### Defining ints, floats and strings

You can define ints, floats and strings simply as attributes:
```xml
< ... fieldName="fieldValue" ... >
```

Or as child elements:

```xml
<fieldName>fieldValue<fieldName/>
```

#### Defining references

As attributes:
```xml
< ... fieldName="referencedObjectName" ... >
```

Or as child elements:

```xml
<fieldName>referencedObjectName<fieldName/>
```

#### Defining single values

Single values must be defined as child element with field name. Their fields should be defined like core data object fields. For example:

```xml
<CoreDataObject ...>
  <singleValueField attributeFields...>
    ... child element fields ...
  </singleValueField>
</CoreDataObject>
```

#### Defining values arrays

Values arrays must be defined as child elements in this way:

```xml
<fieldName>
  <ArrayTypeName ...>...</ArrayTypeName>
  <ArrayTypeName ...>...</ArrayTypeName>
  ...
</fieldName
```

For example:

```xml
<types>
<MapObjectPrefab image="Images/DefaultObject0.png" width="2" height="2" />
    <MapObjectPrefab image="Images/DefaultObject1.png" width="2" height="3" />
    <MapObjectPrefab image="Images/DefaultObject2.png" width="3" height="2" />
    <MapObjectPrefab image="Images/DefaultObject3.png" width="1" height="2" />
</types>
```

If array elements are ints, floats or strings, their definition is almost the same as their child element field definition:

```xml
<integers>
  <int>1</int>
  <int>2</int>
  <int>3</int>
</integers>
```

#### Defining reference arrays

Reference array must be defined in almost the same way as value array:
```xml
<myReferenceArray>
  <reference>ReferenceNameOne</reference>
  <reference>ReferenceNameTwo</reference>
  <reference>ReferenceNameThree</reference>
</myReferenceArray>
```

# Example CMake project

There is simple example cmake [project](https://github.com/KonstantinTomashevich/GamedevResourcePackerTestProject) that shows GamedevResourcePacker usage basics.
