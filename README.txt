TAXONOMY MENU
=============

INTRO
=====
This module adds links to taxonomy terms to the global navigation menu.


INSTALLATION
============
1) Place this module directory into your Drupal modules directory.

2) Enable the taxonomy_menu module in Drupal, at:
   administration -> site configuration -> modules (admin/build/modules)

3) Choose which vocabularies to appear in the menu at:
   administration -> content management -> taxonomy
   (admin/content/taxonomy)

4) Edit the vocabulary or create a new one.

CONFIGURATION
=============
All configuration options are on the vocabulary's edit screen 
admin/content/taxonomy/edit/vocabulary/$vid)
Options:
 Menu: Select which menu the vocabulary's terms should appear under
 
 Menu Path Type: Select how the url for the term path should be created.  
  This is extendable using hook_taxonomy_menu_path().
  See developers documentation for more information.
 
 Syncronise changes to this vocabulary: If selected, the menu will auto update when you
  change a node or term.  Recommened to always have this selected.
 
 Display Number of Nodes: Displays the number of nodes next to the term in the menu.
 
 Hide Empty Terms: Does not create menu links for terms with no nodes attached to them.
 
 Item for Vocabulary: Create a menu link for the vocabulary.  This will be the parent menu item.
 
 Auto Expand Menu Item: Enables the 'Expand' option when creating the menu links.  
  This is useful if using suckerfish menus in the primary links.
 
 Select to rebuild the menu on submit: Deletes all of menu items and relationships between the menu and terms
  and recreates them from the vocabulary's terms.  This will create new mlid's for each item, so be careful
  if using other modules to extend the menu functionality.
 

VIEWS/PATHAUTO
=========================
This will enable the category/vid/tid/tid path but using the alias instead of the number.

1) Create a view.
   * The first argument is $vid and the second is $tid and allow multiples.
   * Create a page display with the path 'category/%' (or whatever you are going to use as the base of the path)

2) Change the default path patterm for taxonomy on admin/build/path/pathauto to 'category/[vocab-raw]/[catpath-raw]'
   Use the same base path that you did when you setup the view.
   
2) Bulk updated the alias at admin/build/path/pathauto.

3) Edit the Vocabulary
   * If you already have a menu then select 'Select to rebuild the menu on submit.' and press 'Save'
   * If you haven't created a menu then do so now
   
Now the menu links will be created as category/<vocab name>/<term name>/<term name>/...
The view will handle how the content is displayed.

NOTES
=====
 * Taxonomy Menu does not handle the menu call backs.  It only creates the links to the menus.
 * The router item must be created before Taxonomy Menu creates the links.  Failure to so so
   will cause the menu items to not be created.
 * Router items can be created by either a view or another modules hook_menu.
 * If using Path Auto, be sure that you have either a view or hook_menu setup to recive the callback.



Advanced Breadcrumbs can be controled by Taxonomy Breadcrumb (http://drupal.org/project/taxonomy_breadcrumb)
