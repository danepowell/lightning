<?php

/**
 * @file
 * Contains \MediaLibrarySubContext.
 */

use Drupal\DrupalExtension\Context\DrupalSubContextBase;

/**
 * Contains step definitions for testing the CKEditor media widget.
 */
class MediaLibrarySubContext extends DrupalSubContextBase {

  /**
   * The Mink context.
   *
   * @var \Drupal\DrupalExtension\Context\MinkContext
   */
  protected $minkContext;

  /**
   * Pre-scenario hook.
   *
   * @BeforeScenario
   */
  public function gatherContexts() {
    $this->minkContext = $this->getContext('\Drupal\DrupalExtension\Context\MinkContext');
  }

  /**
   * Waits for a JavaScript condition to become truthy.
   *
   * @param string $expression
   *   The JavaScript expression to wait for.
   */
  protected function awaitExpression($expression) {
    $this->getSession()->wait(10000, $expression);
  }

  /**
   * Waits for an element to exist.
   *
   * @param string $selector
   *   The element's CSS selector.
   * @param int $timeout
   *   How many seconds to wait before timing out.
   */
  protected function awaitElement($selector, $timeout = 10) {
    $this->getSession()
      ->wait($timeout * 1000, 'document.querySelector("' . addslashes($selector) . '")');
  }

  /**
   * Opens the media browser, obviously.
   *
   * @param string $button
   *   (optional) The embed button ID.
   *
   * @When I open the media browser
   */
  public function openMediaBrowser($button = 'media_browser') {
    /** @var \CkEditorSubContext $ck_context */
    $ck_context = $this->getContext('\CkEditorSubContext');
    $ck_context->execute('editdrupalentity', NULL, ['id' => $button]);

    $this->minkContext->iWaitForAjaxToFinish();
    $frame = 'entity_browser_iframe_' . $button;
    $this->awaitElement('iframe[name="' . $frame . '"]', 30);
    $this->getSession()->switchToIFrame($frame);
  }

  /**
   * Selects an item from the media library.
   *
   * @param int $n
   *   The one-based index of the item to select.
   *
   * @When I select item :n in the media browser
   */
  public function selectNthItem($n) {
    sleep(5);
    $row = floor($n / 4) + 1;
    $column = ($n % $row) + 1;

    $selector = '.view-media .row-' . $row . ' .col-' . $column;
    /** @var \LightningSubContext $lightning */
    $lightning = $this->getContext('\LightningSubContext');
    $lightning->clickElementBySelector($selector);
  }

  /**
   * Completes the media browser selection.
   *
   * @When I complete the media browser selection
   */
  public function completeSelection() {
    /** @var \Behat\Mink\Session $session */
    $session = $this->getSession();
    $session->switchToWindow();
    $session->executeScript('window.frames.entity_browser_iframe_media_browser.document.forms[0].elements.op.click()');
    $this->awaitExpression('!window.frames.entity_browser_iframe_media_browser');
    $this->minkContext->iWaitForAjaxToFinish();
    $this->awaitElement('form.entity-embed-dialog');
    $session->executeScript('document.querySelector("form.entity-embed-dialog").elements.op[1].click()');
    $this->minkContext->iWaitForAjaxToFinish();
  }

  /**
   * Waits for the inline entity form (containing required fields) to exist.
   */
  protected function awaitEntityForm() {
    $this->minkContext->iWaitForAjaxToFinish();
    $this->awaitExpression('document.getElementById("ief-target").textContent.length');
  }

  /**
   * Uploads a file in the media browser.
   *
   * @param string $file
   *   The path to the file, relative to the test files directory.
   *
   * @When I upload :file
   */
  public function upload($file) {
    $this->minkContext->clickLink('Upload');
    $this->minkContext->attachFileToField('File', $file);
    $this->awaitEntityForm();
  }

  /**
   * Enters an embed code in the media browser.
   *
   * @param string $code
   *   The embed code.
   *
   * @When I enter embed code :code
   */
  public function embed($code) {
    $this->minkContext->clickLink('Create Embed');
    $this->minkContext->fillField('embed_code', $code);
    // The change event, which triggers AJAX, is fired automatically after 600
    // milliseconds.
    sleep(1);
    $this->awaitEntityForm();
  }

}
