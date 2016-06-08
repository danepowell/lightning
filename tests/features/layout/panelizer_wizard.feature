@lightning @layout @api @rc7
Feature: Panelizer Wizard

  Scenario: Landing page default layout has the proper Content context
    Given I am logged in as a user with the layout_manager role
    And I visit "/admin/structure/panels/panelizer.wizard/node__landing_page__default__default/select_block"
    Then I should see "Authored by"

