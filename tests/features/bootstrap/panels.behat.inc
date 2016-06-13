<?php

/**
 * @file
 * Contains \PanelsSubContext.
 */

use Behat\Mink\Element\NodeElement;
use Drupal\DrupalExtension\Context\DrupalSubContextBase;
use Drupal\DrupalExtension\Context\DrupalSubContextInterface;

/**
 * Contains step definitions for working with Panels and panelized pages.
 */
class PanelsSubContext extends DrupalSubContextBase implements DrupalSubContextInterface {

  /**
   * The Mink context.
   *
   * @var \Drupal\DrupalExtension\Context\MinkContext
   */
  protected $minkContext;

  /**
   * The Lightning subcontext.
   *
   * @var \LightningSubContext
   */
  protected $lightning;

  /**
   * Pre-scenario hook.
   *
   * @BeforeScenario
   */
  public function gatherContexts() {
    $this->minkContext = $this->getContext('\Drupal\DrupalExtension\Context\MinkContext');
    $this->lightning = $this->getContext('LightningSubContext');
  }

  /**
   * Applies Panelizer to a node type.
   *
   * @param string $bundle
   *   The node type ID.
   *
   * @Given I have applied Panelizer to the :bundle node type
   * @Given I have panelized the :bundle node type
   *
   * @When I apply Panelizer to the :bundle node type
   * @When I panelize the :bundle node type
   */
  public function panelize($bundle) {
    $this->minkContext->assertAtPath("admin/structure/types/manage/$bundle/display");
    $this->minkContext->checkOption('panelizer[enable]');
    $this->minkContext->checkOption('panelizer[custom]');
    $this->minkContext->pressButton('Save');
  }

  /**
   * Removes Panelizer from a node type.
   *
   * @param string $bundle
   *   The node type ID.
   *
   * @Given I have removed Panelizer from the :bundle node type
   * @Given I have unpanelized the :bundle node type
   *
   * @When I remove Panelizer from the :bundle node type
   * @When I unpanelize the :bundle node type
   */
  public function unpanelize($bundle) {
    $this->minkContext->assertAtPath("admin/structure/types/manage/$bundle/display");
    $this->minkContext->uncheckOption('panelizer[enable]');
    $this->minkContext->uncheckOption('panelizer[custom]');
    $this->minkContext->pressButton('Save');
  }

  /**
   * Places a block into a Panels layout.
   *
   * @param string $plugin_id
   *   The block plugin ID.
   * @param string $category
   *   The block's category.
   *
   * @When I place the :plugin_id block from the :category category
   */
  public function placeBlock($plugin_id, $category) {
    $this->lightning->clickElementBySelector('a[title = "Manage Content"]');
    $this->minkContext->iWaitForAjaxToFinish();

    $this->lightning->clickElementBySelector('a[data-category = "' . $category . '"]');
    $this->minkContext->iWaitForAjaxToFinish();

    $this->lightning->clickElementBySelector('a[data-plugin-id = "' . $plugin_id . '"]');
    $this->minkContext->iWaitForAjaxToFinish();

    $this->lightning->clickElementBySelector('[value = "Add"]');
    $this->minkContext->iWaitForAjaxToFinish();
  }

  /**
   * Places a block into a panelizer layout via Wizard.
   *
   * @param string $block_name
   *   The text name of the block.
   * @param string $region
   *   The name of the region in which to place the block.
   *
   * @When I place the :block_name into the :region panelizer region
   */
  public function placePanelizerBlock($block_name, $region) {
    $this->minkContext->clickLink('Add new block');
    $this->minkContext->iWaitForAjaxToFinish();

    $this->minkContext->clickLink($block_name);
    $this->minkContext->iWaitForAjaxToFinish();

    $this->minkContext->selectOption('Region', $region);
    $this->minkContext->iWaitForAjaxToFinish();

    $this->minkContext->pressButton('Add block');
    $this->minkContext->iWaitForAjaxToFinish();
  }

  /**
   * Removes a block from the panelizer layout via the Wizard.
   *
   * Assumes that exactly one block with the given name exists in the given
   * region.
   *
   * @param string $block_name
   *   The text name of the block.
   * @param string $region
   *   The name of the region in which the block is currently placed.
   *
   * @When I remove the :block_name block from the :region panelizer region
   */
  public function removePanelizerBlock($block_name, $region) {
    $row = $this->getBlockRow($block_name, $region);
    if ($row) {
      $uuid = substr($row->getAttribute('data-drupal-selector'), 12);
      $this->minkContext->visit('/admin/structure/panels/panelizer.wizard/node__landing_page__full__two_column/delete/' . $uuid);
      $this->minkContext->pressButton('Delete');
      $this->minkContext->visit('/admin/structure/panelizer/edit/node__landing_page__full__two_column/content');
      $this->minkContext->pressButton('Update and save')
    }
  }

  /**
   * Returns the table row for a specific block in a specific region.
   *
   * @param string $block_label
   *   The label of the block to locate.
   * @param string $region
   *   The machine name of the region in which the block is expected to be.
   *
   * @return \Behat\Mink\Element\NodeElement|NULL
   *   The row element, or NULL if one was not found.
   */
  protected function getBlockRow($block_label, $region) {
    $page = $this->getSession()->getPage();

    // array_map() callback. Traverses from a region select list to the table
    // row that contains it.
    $row_map = function (NodeElement $select) {
      return $select
        // The containing DIV.
        ->getParent()
        // The table cell.
        ->getParent()
        // The table row.
        ->getParent();
    };

    $elements = $page->findAll('css', 'table#blocks tr > td > div > select.block-region-select');
    $elements = array_filter($elements, function (NodeElement $element) use ($region) {
      return $element->getValue() == $region;
    });

    /** @var NodeElement $row */
    foreach (array_map($row_map, $elements) as $row) {
      // The first cell is the one with the label; find() will return the first
      // matched element, which should be the first cell.
      $row_label = $row->find('css', 'td')->getText();
      if (trim($row_label) == $block_label) {
        return $row;
      }
    }
  }

  /**
   * Saves the current Panels layout as a custom layout.
   *
   * @When I save the layout
   */
  public function saveCustomLayout() {
    $this->lightning->clickElementBySelector('a[title = "Save"]');
    $this->minkContext->iWaitForAjaxToFinish();

    $this->lightning->clickElementBySelector('a.panelizer-ipe-save-custom');
    $this->minkContext->iWaitForAjaxToFinish();
  }

  /**
   * Asserts that a block with a specific plugin ID is present.
   *
   * @param string $plugin_id
   *   The block plugin ID.
   *
   * @Then I should see a :plugin_id block
   */
  public function assertBlock($plugin_id) {
    $this->assertSession()->elementExists('css', 'div[data-block-plugin-id = "' . $plugin_id . '"]');
  }

  /**
   * Asserts that a block with a specific plugin ID is not present.
   *
   * @param string $plugin_id
   *   The block plugin ID.
   *
   * @Then I should not see a :plugin_id block
   */
  public function assertNotBlock($plugin_id) {
    $this->assertSession()->elementNotExists('css', 'div[data-block-plugin-id = "' . $plugin_id . '"]');
  }

  /**
   * Asserts that a block has contextual links.
   *
   * @param string $plugin_id
   *   The block plugin ID.
   * @param string $link_class
   *   (optional) The class for a specific contextual link to assert.
   *
   * @Then the :plugin_id block should have contextual links
   * @Then the :plugin_id block should have a :link_class contextual link
   * @Then I should see a :plugin_id block with contextual links
   * @Then I should see a :plugin_id block with a :link_class contextual link
   */
  public function assertBlockContextualLinks($plugin_id, $link_class = NULL) {
    $selector = 'div[data-block-plugin-id = "' . $plugin_id . '"] ul.contextual-links';
    if ($link_class) {
      $selector .= ' li.' . $link_class;
    }
    $this->assertSession()->elementExists('css', $selector);
  }

}
