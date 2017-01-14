
/**
    @page Overview Overview

    @tableofcontents


    The wxVirtualDataViewCtrl is a control designed to display and edit a very large amount of items.
    By "very large", it is meant several millions.
    Moreover, the memory required to manage this amount of items should be constant.

    The assumptions and desired features for the wxVirtualDataViewCtrl are:
        - ASSUMPTION: the data to be displayed is already available (either in memory or inside a database / file / ...)
        - ASSUMPTION: memory is scarce, computing power is available
        - ASSUMPTION: the data is organized in some sort of tree structure
                      (a list is considered a special case of tree, with 1 root item and N direct children)
        - ASSUMPTION: it is not possible / desirable to use delayed tree creation (and data is already available anyway)
        - FEATURE : the control should use a constant amount of memory (that is, not dependent on the amount of items)
        - FEATURE : due to the large amount of items, sorting and filtering should be built-in
        - FEATURE : run-time performance (scrolling, selecting, editing, viewing) should be at least as fast as a tree control with a few items

    Several alternatives have been reviewed before starting a new control from scratch:
        - use the control wxTreeListCtrl from wxCode
        - use the wxDataViewCtrl

    Although both controls are fine, and very useful on their own, they displayed runtime performances which were not sufficient.
    Namely:

        wxTreeListCtrl: this is a clean tree control implementation. It has been modified to be as much virtual as possible.
                        The following caveats have been found:
                            - it stores a wxTreeItemId for each item in the tree. This leads to a large memory usage.
                              In addition, the wxTreeItemId stores a vector, leading to large memory fragmentation (many vectors -> many allocated blocks).
                              The later can be avoided using some sort of data model, but requires heavy patching of the wxTreeListCtrl
                              So the memory usage increases steadily with the amount of items

                            - When the amount of items is large (> 1 million), the control becomes slow to scroll. This is especially true when the
                              items have a large amount of children (nearly flat tree). Runtime performance during display and scrolling becomes quadratic

                            - it relies heavily on recursive function calls. Although generally not a problem, it brings the risk of a stack overflow (crash)

        wxDataViewCtrl: the newest control in wxWidgets, with a clean MVC architecture. It has however the following caveats:
                            - the control stores some kind of internal tree. This leads to unbounded memory usage, similar to wxTreeListCtrl.
                              This is in addition internal to the control, so the user cannot replace it / optimize it.
                              This internal tree stores as well the expanded/collapsed state of the nodes

                            - When the amount of direct children of 1 item is large (> 10 000), the scrolling & display of the item becomes slow

                            - no display of tree lines.

        Both controls:
        there are (at least) 2 hard limits on the amount of items displayable:
                            - the vertical scrollbar is limited to 2^31 units (signed 32 bits long : approximately 2 billions units).
                                This is a Win32 platform limitation, not wxWidgets.
                                However, the scrollbar units are mapped to pixels of the client area. Since 1 item is generally 20 pixels high, there is
                                a hard limit to approximately 100 millions items displayable (should be enough, even for me !)
                            - the wxDC on Win32 cannot display an area larger than (-2^27, 2^27) pixels in both directions. Still using 20 pixels per item,
                                we hit a limit of approximately 6,5 millions items displayable : this can be a problem (I have models with 10 millions items).

        So due to their hierarchical architecture, these controls are not optimal when displaying very large models (> 1 million items).

        Patching wxDataViewCtrl for avoiding the 2nd hard limit proved very time consuming and tricky. In addition, it was not solving the runtime performance problem

    For these reasons, the wxVirtualDataViewCtrl has been written.
    It uses a clean Model/View architecture, namely:
        - the control itself does not store the data. It uses a "Data Model" for getting information about the items, and a "State Model"
          for storing the state of the items (expanded/collapsed, selected, checked, enabled/disabled)
        - the memory footprint for the control itself is constant : the bulk of the memory is used by the "Data Model" and the "State Model",
          which should be provided by the user
        - run-time speed : care has been taken to get acceptable speed on common operation (scrolling/sorting/display). When a time consuming operation
          is performed, it is generally implemented in the "Data Model" or "State Model", allowing the user to optimize them for his/her case when possible ("know your data")
        - sorting and filtering is built-in, by using "proxy data models". This allows to defines a hierarchy of models, and to apply several
          filters on a data model for having custom sorting/filtering/categorizing/...
        - the wxWidgets control is decoupled from the programming logic via a "Model Renderer". This has 2 benefits:
            - split a very big class in 2 smaller classes (easier to understand and maintain)
            - hot swap the renderer without destroying/recreating the control. This behaviour can be seen in the wxListCtrl, where "report view" / "icon view"
                can be swapped at run-time
**/
