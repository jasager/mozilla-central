/* Any copyright is dedicated to the Public Domain.
   http://creativecommons.org/publicdomain/zero/1.0/ */

/**
 * Check extension-added tab actor lifetimes.
 */

var gTab1 = null;
var gTab1Actor = null;

var gClient = null;

function test()
{
  DebuggerServer.addActors("chrome://mochitests/content/browser/browser/devtools/debugger/test/testactors.js");

  let transport = DebuggerServer.connectPipe();
  gClient = new DebuggerClient(transport);
  gClient.connect(function (aType, aTraits) {
    is(aType, "browser", "Root actor should identify itself as a browser.");
    get_tab();
  });
}

function get_tab()
{
  gTab1 = addTab(TAB1_URL, function() {
    attach_tab_actor_for_url(gClient, TAB1_URL, function(aGrip) {
      gTab1Actor = aGrip.actor;
      ok(aGrip.testTabActor1, "Found the test tab actor.")
      ok(aGrip.testTabActor1.indexOf("testone") >= 0,
         "testTabActor's actorPrefix should be used.");
      gClient.request({ to: aGrip.testTabActor1, type: "ping" }, function(aResponse) {
        is(aResponse.pong, "pong", "Actor should respond to requests.");
        close_tab(aResponse.actor);
      });
    });
  });
}

function close_tab(aTestActor)
{
  removeTab(gTab1);
  try {
    gClient.request({ to: aTestActor, type: "ping" }, function (aResponse) {
      is(aResponse, undefined, "testTabActor1 didn't go away with the tab.");
      finish_test();
    });
  } catch (e) {
    is(e.message, "'ping' request packet has no destination.",
       "testTabActor1 should have gone away with the tab.");
    finish_test();
  }
}

function finish_test()
{
  gClient.close(function () {
    finish();
  });
}
