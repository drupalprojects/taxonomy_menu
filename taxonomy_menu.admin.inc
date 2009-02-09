<?php
// $Id$

/**
 * @author Jonathan Chaffer   <jchaffer@structureinteractive.com>
 * @author Bruno Massa        <http://drupal.org/user/67164>
 * @file taxonomy_menu.inc
 * It Generates menu links for all taxonomy terms
 */

/**
 * Admin area. Configure the module, setting which
 * vocabularies will be converted into menus items
 *
 * @return
 *   Array. The form fields.
 * @ingroup forms
 */
function taxonomy_menu_admin_settings() {
  $form['taxonomy_menu_display_num'] = array(
    '#default_value'  => variable_get('taxonomy_menu_display_num', FALSE),
    '#description'    => t('If checked, number of nodes per term will be displayed in the menu.'),
    '#title'          => t('Display number of nodes per term'),
    '#type'           => 'checkbox',
  );
  $form['taxonomy_menu_hide_empty'] = array(
    '#default_value'  => variable_get('taxonomy_menu_hide_empty', FALSE),
    '#description'    => t('If checked, only taxonomy terms with associated nodes will be shown in the menu.'),
    '#title'          => t('Hide Empty Terms'),
    '#type'           => 'checkbox',
  );
  $form['taxonomy_menu_display_descendants'] = array(
    '#default_value'  => variable_get('taxonomy_menu_display_descendants', TRUE),
    '#description'    => t('If checked, then when a term is selected all nodes belonging to subterms are also displayed.'),
    '#title'          => t('Display descendants'),
    '#type'           => 'checkbox',
  );

  return system_settings_form($form);
}

