<?php

/**
 * @file
 * admin section for taxonomy menu
 */

define('TAXONOMY_MENU_BASE_PATH', 'admin/build/taxonomy_menu/');

/**
 * page callback for admin/build/taxonomy_menu
 * @TODO Turn this into a theme function
 */
function taxonomy_menu_group_list() {
  //get a list of the menu groups
  $groups = taxonomy_menu_get_menu_groups();
  //cycle through the menu groups and build the display
  foreach($groups as $group) {
    //add row for the group
    $rows[] = array(
      '<strong>Menu Group:</strong>  '. check_plain($group->name),
      l(t('Edit'), TAXONOMY_MENU_BASE_PATH .'group/'. $group->mgid. '/edit'),
      l(t('Rebuild'), TAXONOMY_MENU_BASE_PATH .'group/'. $group->mgid. '/rebuild'),
      l(t('Delete'), TAXONOMY_MENU_BASE_PATH .'group/'. $group->mgid. '/delete'),
    );

    //cycle through the menu group items and display
    $term_sets = taxonomy_menu_get_term_sets_by_group($group->mgid);
    taxonomy_menu_term_set_table($term_sets, $rows);

    //add an empty line
    $rows[] = array();
  }

  if (empty($rows)) {
    return t('No Menu Groups');
  }
  else {
    return theme('table', array(), $rows);
  }
}


/**
 * Formats the list of Term Sets for a given Menu Group
 * @param object $term_sets
 * @return
 */
function taxonomy_menu_term_set_table($term_sets, &$rows) {
  if ($term_sets) {
    foreach($term_sets as $term_set) {
      $term_set_path = $term_set->mgid .'/'. $term_set->tsid;
      $rows[] = array(
        '-Term Set: '. check_plain($term_set->name),
        l(t('Edit'), TAXONOMY_MENU_BASE_PATH .'termset/'. $term_set->tsid .'/edit'),
        l(t('Delete'), TAXONOMY_MENU_BASE_PATH .'termset/'. $term_set->tsid .'/delete'),
      );
    }
    //Add 'add term set' row
    //$rows[] = array(l(t('Add a Term Set'), TAXONOMY_MENU_BASE_PATH .'termset/add'));
  }
  else {
    $rows[] = array(l(t('No Term Sets.  Edit the menu group associate a Term Set'), TAXONOMY_MENU_BASE_PATH .'gropu/'. $term_set->mgid .'/edit'));
  }
}

/**
 * Callback for List of Term Sets
 * @return
 * @TODO Turn this into a theme function
 */
function taxonomy_menu_term_sets() {
  //get a list of the menu groups
  $term_sets = taxonomy_menu_get_term_sets();
  if ($term_sets) {
    //cycle through the menu groups and build the display
    $rows = array();
    foreach($term_sets as $term_set) {
      //add row for the group
      $rows[] = array(
        '<strong>Term Set:</strong>  '. check_plain($term_set->name),
        l(t('Edit'), TAXONOMY_MENU_BASE_PATH .'termset/'.$term_set->tsid .'/edit'),
        l(t('Delete'), TAXONOMY_MENU_BASE_PATH .'termset/'.$term_set->tsid .'/delete'),
      );

      //cycle through the menu group items and display
      $menu_groups = taxonomy_menu_get_menu_group_by_ts($term_set->tsid);
      taxonomy_menu_group_table($menu_groups, $rows);

      //add an empty line
      $rows[] = array();
    }
  }
  else {
    return t('No Term Sets');
  }

  return theme('table', array(), $rows);
}

/**
 * Formats the list of Term Sets for a given Menu Group
 * @param object $term_sets
 * @return
 */
function taxonomy_menu_group_table($menu_groups, &$rows) {
  if ($menu_groups) {
    foreach($menu_groups as $menu_group) {
      $rows[] = array(
        '-Menu Group: '. check_plain($menu_group->name),
      l(t('Edit'), TAXONOMY_MENU_BASE_PATH .'group/'. $menu_group->mgid .'/edit'),
      );
    }
  }
  else {
    $rows[] = array(t('No Menu Groups for Term Set'));
  }
}

/**
 * Implementation of hook_form().
 *
 */
function taxonomy_menu_group_form($form_state, $mgid = 0) {
  // Add termsets list page to the breadcrumb trail for easier navication
  $breadcrumb = drupal_get_breadcrumb();
  $breadcrumb[] = l(t('Menu Groups'), 'admin/build/taxonomy_menu');
  drupal_set_breadcrumb($breadcrumb);

  // choose a menu to add link items to.
  $menu_items = menu_parent_options(menu_get_menus(), NULL);

  if ($mgid) {
    $menu_group = taxonomy_menu_get_group($mgid);
    $form_state['storage']['menu_group_name'] = $menu_group->name;
    $form_state['storage']['parent_menu_item'] = $menu_group->parent_menu;
    $form_state['storage']['path'] = $menu_group->path;
    $form_state['storage']['mgid'] = $mgid;
    $form_state['storage']['items'] = $menu_group->items;
  }

  //menu group name
  $form['menu_group_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Menu Group Name'),
    '#default_value' => $form_state['storage']['menu_group_name'],
    '#required' => TRUE,
  );
  //select the parent menu item
  $form['parent_menu_item'] = array(
    '#type' => 'select',
    '#title' => t('Parent Menu Item'),
    '#default_value' => $form_state['storage']['parent_menu_item'],
    '#options' => $menu_items,
    '#required' => TRUE,
    '#description' => t('The Parent Menu Item that the Menu Link will be built under.')
  );

  //set the path type for the menu group
  $form['path'] = array(
    '#type' => 'select',
    '#title' => t('Path Type'),
    '#default_value' => $form_state['storage']['path'],
    '#options' => taxonomy_menu_get_paths(),
    '#required' => TRUE,
    '#ahah' => array(
      'event' => 'change',
      'method' => 'replace',
      'path' => 'taxonomy_menu/ahah/path',
      'wrapper' => 'taxonomy-menu-path-options',
      'effect' => 'fade',
    ),
  );
  //Select submit handler if js is not enabled.
  $form['vid_select_submit'] = array(
     '#type' => 'submit',
     '#value' => t('Update the Path Type'),
     '#submit' => array('taxonomy_menu_select_submit'),
     '#attributes' => array('class' => 'no-js'),
  );
    //get the path options
    $form['path_options'] = taxonomy_menu_get_options('PATH', $form_state['storage']['path']);
    //set the title of the field set
    $form['path_options']['#title'] = t('Path Options');
    $form['path_options']['#prefix'] = '<div id = "taxonomy-menu-path-options">';
    $form['path_options']['#suffix'] = '</div>';
    $form['path_options']['#collapsed'] = TRUE;

  $form['term_sets'] = array(
    '#prefix' => '<div id="taxonomy-menu-term-sets">',
    '#suffix' => '</div>',
    '#theme' => 'taxonomy_menu_term_set_table',
  );

  // List of Term Sets
  $term_sets = taxonomy_menu_get_term_sets_by_group($form_state['storage']['mgid']);
  foreach ($term_sets as $term_set) {
    $form['term_sets']['list'][$term_set->tsid]['name'] = array('#value' => t($term_set->name));
    $form['term_sets']['list'][$term_set->tsid]['vocab'] = array('#value' => t($term_set->vocab_name));
    $form['term_sets']['list'][$term_set->tsid]['parent'] = array('#value' => t($term_set->parent->name));
    $form['term_sets']['list'][$term_set->tsid]['remove'] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#submit' => 'taxonomy_menu_group_remove_term_set_submit',
      '#ahah' => array(
        'path' => 'taxonomy_menu/ahah/term_sets_remove/'. $form_state['storage']['mgid'] .'/'. $term_set->tsid,
        'wrapper' => 'taxonomy-menu-term-sets',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );
  }
  // Pass the mgid to to term set theme
  $form['term_sets']['mgid'] = array(
    '#type' => 'value',
    '#value' => $form_state['storage']['mgid'],
  );

  // Only render the term set add if we are editing.
  if ($form_state['storage']['mgid']) {
    // feildset containing the list of term sets and the add button
    $form['term_sets']['term_set_add'] = array(
      '#type' => 'fieldset',
      '#title' => t('Assoicate Term Set to Menu Group'),
      '#tree' => FALSE,
      '#theme' => 'taxonomy_menu_group_term_set_add',
    );

    $form['term_sets']['term_set_add']['term_set'] = array(
      '#type' => 'select',
      '#title' => t('Term Set'),
      '#options' => _taxonomy_menu_get_term_sets_options($form_state['storage']['mgid']),
    );

    $form['term_sets']['term_set_add']['term_set_parent'] = array(
      '#type' => 'select',
      '#title' => t('Parent Term Set Item'),
      '#options' => _taxonomy_menu_get_term_set_parent_item_options($form_state['storage']['items']),
    );
    $form['term_sets']['term_set_add']['term_set_more'] = array(
      '#type' => 'submit',
      '#value' => t('Associate'),
      '#submit' => array('taxonomy_menu_group_term_submit'),
      '#ahah' => array(
        'path' => 'taxonomy_menu/ahah/term_sets',
        'wrapper' => 'taxonomy-menu-term-sets',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );
  }

  //get the group options
  $form['group_options'] = taxonomy_menu_get_options('GROUP', $form_state['storage']['mgid']);
  //set the title of the group field set
  $form['group_options']['#title'] = t('Group Options');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit Handler for taxonomy_menu_group_form
 */
function taxonomy_menu_group_form_submit($form, &$form_state) {

  $menu_group = array(
    'path' => $form_state['values']['path'],
    'name' => $form_state['values']['menu_group_name'],
    'parent_menu' => $form_state['values']['parent_menu_item'],
    'mgid' => $form_state['values']['mgid'],
  );

  //save group settings
  taxonomy_menu_save_menu_group($menu_group);

  //save path options
  if ($form_state['values']['path_options']) {
    _taxonomy_menu_save_options('PATH', $form_state['values']['path_options'], $menu_group['mgid']);
  }

  //save menu group options
  if ($form_state['values']['group_options']) {
    _taxonomy_menu_save_options('GROUP', $form_state['values']['group_options'], $menu_group['mgid']);
  }

  $form_state['redirect'] = 'admin/build/taxonomy_menu/group/'. $menu_group['mgid'] .'/edit';
}

/**
 * Submit Handler for Add Term Set button
 */
function taxonomy_menu_group_term_submit($form, &$form_state) {

  $menu_set = array(
    'mgid' => $form_state['values']['mgid'],
    'tsid' => $form_state['values']['term_set'],
    'parent_item' => $form_state['values']['term_set_parent'],
  );
  taxonomy_menu_save_menu_set_rtln($menu_set);
  $form_values = $form_state['values'];
  unset($form_state['submit_handlers']);
  form_execute_handlers('submit', $form, $form_state);
  $form_state['storage'] = $form_values;
  $form_state['rebuild'] = TRUE;
}

/**
 * Remove a term set from a menu group.
 */
function taxonomy_menu_group_remove_term_set_submit($form, &$form_state) {

  taxonomy_menu_group_remove_term_set($form_state['values']['mgid'], $form_state['values']['term_set']);
  $form_values = $form_state['values'];
  unset($form_state['submit_handlers']);
  form_execute_handlers('submit', $form, $form_state);
  $form_state['storage'] = $form_values;
  $form_state['rebuild'] = TRUE;
}

/**
 * determine if the menu should be disable or enable
 *
 * @param $group object
 * @return link
 */
function _taxonomy_menu_group_display($group) {
  if ($group->enabled) {
    return l(t('Disable'), TAXONOMY_MENU_BASE_PATH . $group->mgid .'/disable');
  }
  else {
    return l(t('Enable'), TAXONOMY_MENU_BASE_PATH . $group->mgid .'/enable');
  }
}



/**
 * Function to save an array of options
 *
 * @param $type
 *  either GROUP/ITEM/GLOBAL/PATH/TERM_SET
 * @param $options
 *  array of options (name => value)
 * @param $type_key
 *  either mgid or mgi_id or path key depending on key of options
 */
function _taxonomy_menu_save_options($type, $options, $type_key) {
  $option = new stdClass;
  foreach ($options as $name => $value) {
    $option->name = $name;
    $option->type = $type;
    $option->type_key = $type_key;
    $option->value = $value;
    taxonomy_menu_save_option($option);
  }
}

/**
 * Menu callback for adding a term set.
 */
function taxonomy_menu_term_set_add($tsid = 0) {
  $vocabs = taxonomy_get_vocabularies();
  if ($vocabs) {
    return drupal_get_form('taxonomy_menu_term_set_form', $vocabs, $tsid);
  }

  return l(t('Please create a vocabulary before creating the Taxonomy Menu'), 'admin/content/taxonomy/add/vocabulary');
}
/**
 * Implementation of hook_form().
 *  Add Term Set form.
 *
 * @param $form_state
 * @param $mgid
 */
function taxonomy_menu_term_set_form($form_state, $vocabs, $tsid = 0) {
  // Add termsets list page to the breadcrumb trail for easier navication
  $breadcrumb = drupal_get_breadcrumb();
  $breadcrumb[] = l(t('Term Sets'), 'admin/build/taxonomy_menu/termset');
  drupal_set_breadcrumb($breadcrumb);

  // If form_state['values'] is filled in then the form has alredy been submitted once
  // so pull out the

  //if tsid is passed then we are editing a term set.
  if ($tsid) {
    $term_set = taxonomy_menu_get_term_set($tsid);
    $form['tsid'] = array('#type' => 'value', '#value' => $tsid);
    $form_state['storage']['title'] = $term_set->name;
    $form_state['storage']['vid'] = $term_set->vid;
    $form_state['storage']['items'] = $term_set->items;
    // Extract the tids from the term object
    foreach ($term_set->items as $term) {
      $form_state['storage']['terms'][] = $term->tid;
    }
  }

  // If the form_values array if filled in then that instead
  if (isset($form_state['values'])) {
    $form_state['storage'] = $form_state['values'];
  }

  // Set the cache to true to create a $form_state cache on submit
  $form['#cache'] = TRUE;

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $form_state['storage']['title'],
    '#required' => TRUE,
  );
  //get a list of the vocabularies
  foreach ($vocabs as $vocab) {
    $vocab_items[$vocab->vid] = $vocab->name;
  }
  array_unshift($vocab_items, '-Select vocabulary-');

  //select the vocabulary for the menu group
  $form['vid'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary'),
    '#required' => TRUE,
    '#options' => $vocab_items,
    '#default_value' => $form_state['storage']['vid'],
    '#ahah' => array(
      'event' => 'change',
      'method' => 'replace',
      'path' => 'taxonomy_menu/ahah/tids',
      'wrapper' => 'taxonomy-menu-tids',
      'effect' => 'fade',
    ),
  );
  //Select submit handler if js is not enabled.
  $form['vid_select_submit'] = array(
     '#type' => 'submit',
     '#value' => t('Update the Term Tree'),
     '#submit' => array('taxonomy_menu_select_submit'),
     '#attributes' => array('class' => 'no-js'),
  );

  $form['terms_wrapper'] = array(
    '#prefix' => '<div id="taxonomy-menu-tids">',
    '#suffix' => '</div>',
  );
  $form['terms_wrapper']['terms'] = array(
    '#type' => 'taxonomy_manager_tree',
    '#vid' => $form_state['storage']['vid'],
    '#title' => t('Select terms'),
    '#parent' => 0,
    '#multiple' => TRUE,
    '#add_term_info' => TRUE,
    '#default_value' => $form_state['storage']['terms'],
  );

  $form['sync_method'] = array(
    '#type' => 'select',
    '#title' => t('Sync Method'),
    '#required' => TRUE,
    '#options' => array('-Select-'),
    '#description' => t('TODO'),
  );

  $form['term_set_options'] = taxonomy_menu_get_options('TERM_SET', $tsid);
  //set the title of the group field set
  $form['term_set_options']['#title'] = t('Term Set Options');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  // Redirect back to the term_set list
  $form['#redirect'] = 'admin/build/taxonomy_menu/termset';
  return $form;
}

/**
 * Submit handler for taxonomy_term_set_form
 */
function taxonomy_menu_term_set_form_submit($form, &$form_state) {

  //Do not run the submit for the AHAH callback
  if ($form_state['clicked_button']['#value'] === t('Save')) {

    // Get the current Term Set ID from The hidden Value.
    $term_set['tsid'] = $form_state['values']['tsid'];

    // Update with the New Values.
    $term_set['name'] = $form_state['values']['title'];
    $term_set['vid'] = $form_state['values']['vid'];

    // If a new term set then save to get the term set id
    if (!$form_state['values']['tsid']) {
    // Save the Term Set
      taxonomy_menu_save_term_set($term_set);
    }

    // Build the term tree
    $term_set['items'] = taxonomy_menu_build_tree($form_state['values']['vid'],
      $form_state['values']['terms']['selected_terms'], $term_set['tsid']);

    // Save the Term Set
    taxonomy_menu_save_term_set($term_set);

    //save TERM_SET options
    _taxonomy_menu_save_options('TERM_SET', $form_state['values']['term_set_options'], $term_set['tsid']);

    // Set redirect
    $form_state['#redirect'] = 'admin/build/taxonomy_menu/termset';
  }
}

/**
 * Submit Handler for select when js is not enabled.
 */
function taxonomy_menu_select_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  unset($form_state['submit_handlers']);
  form_execute_handlers('submit', $form, $form_state);
  $form_state['storage'] = $form_values;
  $form_state['rebuild'] = TRUE;
}

/**
 * AHAH helper
 */
function taxonomy_menu_ahah_helper() {
  $form_state = array('storage' => NULL, 'submitted' => TRUE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form['#post'] = $_POST;
  $form['#redirect'] = FALSE;
  $form['#programmed'] = FALSE;
  $form_state['post'] = $_POST;
  $form['#validate'] = NULL;
  $form['#submit'] = NULL;
  // For customly set #validate and #submit handlers.
  $form_state['submit_handlers'] = NULL;
  $form_state['validate_handlers'] = NULL;
  _taxonomy_menu_ahah_disable_validation($form);
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  return $form;
}

/**
 * AHAH Helper 2
 */
function taxonomy_menu_ahah_element_helper($changed_elements) {
  unset($changed_elements['#prefix'], $changed_elements['#suffix']);
  $javascript = drupal_add_js(NULL, NULL, 'header');
  drupal_json(array(
    'status'   => TRUE,
    'data'     => theme('status_messages') . drupal_render($changed_elements), // rebuild just the part that needs to be changed
    'settings' => call_user_func_array('array_merge_recursive', $javascript['setting']),
  ));
}

/**
 * Ajax callback for Term Set Form
 */
function taxonomy_menu_ahah_tids() {
  $form = taxonomy_menu_ahah_helper();

  //get the changed elements
  taxonomy_menu_ahah_element_helper($form['terms_wrapper']);

}

/**
 * AHAH callback for Path Options
 */
function taxonomy_menu_ahah_path() {
  $form = taxonomy_menu_ahah_helper();

  //get the changed elements
  taxonomy_menu_ahah_element_helper($form['path_options']);
}

/**
 * AHAH callback for associating a term set to a menu group
 */
function taxonomy_menu_ahah_term_sets() {
  $form = taxonomy_menu_ahah_helper();

  //get the changed elements
  taxonomy_menu_ahah_element_helper($form['term_sets']);
}

/**
 * AHAH callback for removing a term set from a menu group
 */
function taxonomy_menu_ahah_term_set_remove($mgid, $tsid) {
  taxonomy_menu_group_remove_term_set($mgid, $tsid);
  taxonomy_menu_ahah_term_sets();
}

/**
 * Disable the validation so the AHAH can be returned.
 * Borrowed from the AHAH_HELPER module.
 */
function _taxonomy_menu_ahah_disable_validation(&$form) {
  foreach (element_children($form) as $child) {
    $form[$child]['#validated'] = TRUE;
    _taxonomy_menu_ahah_disable_validation($form[$child]);
  }
}

/**
 * Helper Function to build the tree array from tids
 * @param $tids array()
 * @return tree array
 */
function taxonomy_menu_build_tree($vid, $tids, $tsid) {
  $tree = taxonomy_get_tree($vid);
  foreach ($tree as $key => $term) {
    // Check to see if the the term is in the tids array
    if (!in_array($term->tid, $tids)) {
      unset($tree[$key]);
    }
    else {
      $tree[$key]->parent = $tsid .':'. $term->parents[0];
      unset($tree[$key]->parents);
      $tree[$key]->tsid = $tsid;
    }
  }
  return $tree;
}

function taxonomy_menu_group_delete_confirm(&$form_state, $mgid) {
  $form['mgid'] = array(
    '#type' => 'value',
    '#value' => $mgid,
  );
  $menu_group = taxonomy_menu_get_group($mgid);
  return confirm_form($form,
    t('Are you sure you want to delete %title?  This will also delete all of the related Menu Items', array('%title' => $menu_group->name)),
    'admin/build/taxonomy_menu',
    t('This action cannot be undone'),
    t('Delete'),
    t('Cancel')
  );
}

function taxonomy_menu_group_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    taxonomy_menu_group_delete($form_state['values']['mgid']);
  }
  $form_state['redirect'] = 'admin/build/taxonomy_menu';
}

function taxonomy_menu_term_set_delete_confirm(&$form_state, $tsid) {
  $term_set = taxonomy_menu_get_term_set($tsid);
  $form['term_set'] = array(
    '#type' => 'value',
    '#value' => $term_set,
  );
  $form['#redirect'] = 'admin/build/taxonomy_menu/termset';

  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => check_plain($term_set->name))),
    drupal_get_destination(),
    t('This action cannot be undone'),
    t('Delete'),
    t('Cancel')
    );
}

function taxonomy_menu_term_set_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    taxonomy_menu_term_set_delete($form_state['values']['term_set']->tsid);
    drupal_set_message(t('%title has been deleted.', array('%title' => check_plain($form_state['values']['term_set']->name))), 'status');
  }
}

/**
 * Save the Menu Group to the database
 * @param array $menu_group
 *  'mgid' => mgid, if blank or 0 then a new menu group will be entered
 *  'path' => path function to run to get the path
 *  'name' => diaply name of menu group
 *  'parnent_menu' => parent menu item formated simialr to the return of menu_parent_options().
 *  'term_set_rtln' => function name used to create the relationship.
 *  'items' => tree array terms
 * @return $menu_group by reference with mgid filled in.
 */
function taxonomy_menu_save_menu_group(&$menu_group) {
  // If pased an object, convert it into an array
  if (is_object($menu_group)) {
    $menu_group = (array) $menu_group;
  }
  if ($menu_group['mgid']) {
    drupal_write_record('taxonomy_menu_group', $menu_group, 'mgid');
  }
  else {
    drupal_write_record('taxonomy_menu_group', $menu_group);
  }
  drupal_set_message(t('Menu Group %menu_group has been saved', array('%menu_group' => $menu_group['name'])));
}

/**
 * Save the Term Set to the database
 * @param array $record
 *  'tsid' => termseti id.  if blank or 0 then a new term set will be entered
 *  'vid' => vocab id
 *  'name' => diaply name of menu group/termset
 *  'items' => array of term objects for the term set
 * @return $term_set by reference with tsid filled in.
 */
function taxonomy_menu_save_term_set(&$term_set) {

  if ($term_set['tsid']) {
    drupal_write_record('taxonomy_menu_term_set', $term_set, 'tsid');
  }
  else {
    drupal_write_record('taxonomy_menu_term_set', $term_set);
  }
}


/**
 * Get the menu group info
 */
function taxonomy_menu_get_group($mgid) {
  $menu_group = db_fetch_object(db_query('SELECT * FROM {taxonomy_menu_group} WHERE mgid = %d', $mgid));
  $menu_group->items = unserialize($menu_group->items);
  $menu_group->options = _taxonomy_menu_get_options_by_key('GROUP', $mgid);
  return $menu_group;
}

/**
 * return an array of menu group objects
 */
function taxonomy_menu_get_menu_groups() {
  $output = array();
  $result = db_query('SELECT * FROM {taxonomy_menu_group');
  while ($data = db_fetch_object($result)) {
    $data->items = unserialize($data->items);
    $output[$data->mgid] = $data;
    $output[$data->mgid]->options =  _taxonomy_menu_get_options_by_key('GROUP', $data->mgid);
  }
  return $output;
}

/**
 * get a list of the term sets for a group
 *
 * @param $mglid
 *  menu group id
 */
function taxonomy_menu_get_term_sets_by_group($mgid) {
  $output = array();
  $result = db_query('SELECT * FROM {taxonomy_menu_group_term_set} WHERE mgid = %d', $mgid);
  while ($data = db_fetch_object($result)) {
    // Split up the parent tsid and tid
    list($ptsid, $ptid) = explode(':', $data->parent_item);
    $output[$data->tsid] = taxonomy_menu_get_term_set($data->tsid);
    $output[$data->tsid]->parent = taxonomy_menu_get_term_set($ptsid);
  }
  return $output;
}

/**
 * Get the term set object
 * @param $tsid
 * @return unknown_type
 */
function taxonomy_menu_get_term_set($tsid) {

  $term_set = db_fetch_object(db_query('SELECT * FROM {taxonomy_menu_term_set} WHERE tsid = %d', $tsid));
  $term_set->items = unserialize($term_set->items);
  $term_set->vocab_name = _taxonomy_menu_get_vocab_name($term_set->vid);
  $term_set->options = _taxonomy_menu_get_options_by_key('TERM_SET',$tsid);

  return $term_set;
}

/**
 * Get all of the term sets
 * @return
 */
function taxonomy_menu_get_term_sets() {
  $output = array();
  $result = db_query('SELECT tsid FROM {taxonomy_menu_term_set}');
  while ($data = db_fetch_object($result)) {
    $output[$data->tsid] = taxonomy_menu_get_term_set($data->tsid);
  }
  return $output;
}

/**
 * Get all of the term sets and format for a select
 * @param
 *  mgid menu group id so we do not pull the already assocated term sets
 * @return
 */
function _taxonomy_menu_get_term_sets_options($mgid) {
  $term_sets = array();
  $result = db_query('SELECT * FROM {taxonomy_menu_group_term_set} WHERE mgid = %d', $mgid);
  while ($data = db_fetch_object($result)) {
    $term_sets[] = $data->tsid;
  }
  $output = array();
  $result = db_query('SELECT * FROM {taxonomy_menu_term_set}');
  while ($data = db_fetch_object($result)) {
    // If the term set is not already associated to the menu group
    // then add it to the output array.
    if (!in_array($data->tsid, $term_sets)) {
      $output[$data->tsid] = $data->name;
    }
  }
  return $output;
}

function taxonomy_menu_get_menu_group_by_ts($tsid) {
  $output = array();
  $result = db_query('SELECT * FROM {taxonomy_menu_group_term_set} WHERE tsid = %d', $tsid);
  while ($data = db_fetch_object($result)) {
    $output[$data->mgid] = taxonomy_menu_get_group($data->mgid);
  }
  return $output;
}


/**
 * Save a Term Set/mneugrpou Reltn
 * @param array $menu_set = array('mgid' => number, 'tsid' => number, 'parent_item' => 'tsid:tid')
 * @return
 */
function taxonomy_menu_save_menu_set_rtln($menu_set) {
  // @TODO CHeck for existance first
  drupal_write_record('taxonomy_menu_group_term_set', $menu_set);

  // Get the current menu group
  $menu_group = taxonomy_menu_get_group($menu_set['mgid']);

  // Cycle through the related terms
  $term_set = taxonomy_menu_get_term_set($menu_set['tsid']);
  // Add the new terms to the current menu group
  // Find the depth of the parent item
  if ($menu_set['parent_item']) {
    $depth = $menu_group->items[$menu_set['parent_item']]->depth + 1;
  }
  else {
    $depth = 0;
  }
  foreach ($term_set->items as $key => $item) {
    // Assign the new depth
    $item->depth = $item->depth + $depth;
    unset($item->parents);
    $item->parent = $menu_set['parent_item'];
    $menu_group->items[$item->tsid .':'. $item->tid] = $item;
  }

  // Sort the items
  $menu_group->items = taxonomy_menu_build_menu_tree($menu_group->items);
  // Save the menu group.
  taxonomy_menu_save_menu_group($menu_group);

}

function taxonomy_menu_group_delete($mgid, $name = NULL) {
  // Load the menu group if no name is passed.
  if (!$name) {
    $menu_group = taxonomy_menu_get_group($mgid);
    $name = $menu_group->name;
  }
  //delete the relationsip between the term sets and menu group
  db_query('DELETE FROM {taxonomy_menu_group_term_set} WHERE mgid = %d', $mgid);
  //delete the menu group options
  _taxonomy_menu_options_delete_by_parent($mgid, 'GROUP');
  //delete the menu group
  db_query('DELETE FROM {taxonomy_menu_group} WHERE mgid = %d', $mgid);
  // Inform the user that the menu group has been deleted
  drupal_set_message(t('Menu Group %menu_group has been deleted', array('%menu_group' => $name)));
}

function taxonomy_menu_term_set_delete($tsid) {
  // Get all of the Menu Groups for this term set
  $menu_groups = taxonomy_menu_get_menu_group_by_ts($tsid);
  foreach ($menu_groups as $mgid => $menu_group) {
    // Remove the term set from the group.
    taxonomy_menu_group_remove_term_set($mgid, $tsid);
  }
  //delete the menu item options
  _taxonomy_menu_options_delete_by_parent($tsid, 'ITEM');
  db_query('DELETE FROM {taxonomy_menu_term_set WHERE tsid = %d', $tsid);
}

function _taxonomy_menu_options_delete_by_parent($parent, $type) {
  db_query("DELETE FROM {taxonomy_menu_options} WHERE type_key = '%s' AND type = '%s'", strval($parent), $type);
}

/**
 * Get the terms for the term set items releated to the menu group
 * @param $items
 * @return array (tid => term)
 */
function _taxonomy_menu_get_term_set_parent_item_options($items) {
  $output = array();
  if ($items) {
    foreach ($items as $item) {
      $output[$item->tsid .':'. $item->tid] = str_repeat('-', $item->depth) . $item->name;
    }
  }
  array_unshift($output, t('None'));

  return $output;
}

/**
 * Menu callback to remove Term Set from Menu group
 * @param $mgid
 * @param $tsid
 */
function taxonomy_menu_group_remove_term_set($mgid, $tsid) {

  // Get the current menu group
  $menu_group = taxonomy_menu_get_group($mgid);

  // Cycle through the menu group items and remove the items for the term set
  foreach ($menu_group->items as $key => $item) {
    if ($item->tsid == $tsid) {
      unset ($menu_group->items[$key]);
    }
  }

  // Update the menu group items
  taxonomy_menu_save_menu_group($menu_group);

  // Delete the record from the menu group table
  db_query('DELETE FROM {taxonomy_menu_group_term_set} WHERE mgid = %d AND tsid = %d', $mgid, $tsid);
  drupal_set_message(t('Term Set Removed'), 'status');
}

/**
 * This function orders the items array correctly.
 */
function taxonomy_menu_build_menu_tree($items) {
  if (!$items) {
    return $items;
  }

  // Setup up output array
  $tree = array();
  foreach ($items as $key => $item) {
    $tree[$key] = $item;
    // Get the children
    $tree = array_merge($tree, _taxonomy_menu_build_menu_tree($items, $key));
  }

  return $tree;
}

/**
 * Helper function for taxonomy_menu_build_menu_tree
 */
function _taxonomy_menu_build_menu_tree(&$items, $key) {
  // Return the children for the key
  $output = array();
  foreach ($items as $item_key => $item) {
    if ($item->parent == $key) {
      $output[$item_key] = $item;
      unset($items[$item_key]);
    }
  }

  if (!$output) {
    return array();
  }

  // Sort by the weight
  //$output = new objSorter($output, 'weight');
  usort($output, '_taxonomy_menu_sort_tree');

  // Re add the indexes
  foreach ($ouput as $index => $value) {
    unset($output[$index]);
    $output[$value->tsid .':'. $value->tid];
  }

  return $output;
}

/**
 * Sort Function for building a tree
 */
function _taxonomy_menu_sort_tree($a, $b) {
  if ($a->weight == $b->weight) {
    return 0;
  }

  return ($a->weight < $b->weight) ? -1 : 1;
}
