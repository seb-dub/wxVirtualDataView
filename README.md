
Main documentation:
    see doc

Building:
    sorry, you are on your own. There is a CodeBlocks project file, which compiles
    the library and the examples in 1 single binary. I will do better when I have more time

    Simply add all the files to your project - all the controls are generic, so there should not
    be a per-platform file compilation.
    There a few exceptions to this rule (namely wxwxVirtualHeaderRendererWin32), but there are systematically
    guarded using #if ... #endif, and will not break anything if not present

BUGS:
	- not really bugs, but many performances improvement to do

TODO List:
	- performance : sorting a big tree is slow. This is not the sorting which is slow, but mainly the the scrolling. Caching alleviates the problem, but does not solve it
	- performance : filtering UI. The filtering itselfs is fast enough, but the UI is not responsive when the amount of items is large (> 1000000)
	- compatibility layer: implement the column interface for wxDataViewColumn. Maybe store a duplicate object to allow having both wxDVC columns & wxVirtualDVC columns

Add a CMake makefile


performances optimization
    - ConvertRowToPath : the magic constant should be customizable
    - scrolling up     : some slow down sometimes.
    - selecting: filling the list of old/new items takes time

selecting : take into account data model flags
    selecting: see in wxVirtualTreeModelRenderer & wxVirtualDataViewCtrl

drag / drop

filtering
    Init filter : can be slow -> add a flag for initialization, and recursive initialization

    add filters:
        - date / time / date-time
        - bool
        - NOT
        - BOOLEAN operator : AND / OR / XOR

    filtering UI:
        - add complex filter (AND / OR / XOR) in the form of a tree
        - multi-column filtering
        - UI for RegEx / case sensitive / full match / partial match
        - UI for numeric filter : add arithmetic comparison (<, >, <=, >=, ==, !=, in range, ...)

list view icons (new model renderer)
graph view (VPM style : new model renderer)

more editors / renderers:
    - wrapped text
    - file / dir
    - composite
    - wxBitmapText: something like that, but use an image list.
                    the reason is to improve the speed for sorting/filtering the data in such column
