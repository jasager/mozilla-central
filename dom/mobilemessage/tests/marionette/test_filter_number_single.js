/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

MARIONETTE_TIMEOUT = 60000;

SpecialPowers.addPermission("sms", true, document);
SpecialPowers.setBoolPref("dom.sms.enabled", true);

let manager = window.navigator.mozMobileMessage;
let numberMsgs = 10;
let smsList = new Array();
let defaultRemoteNumber = "+15552227777";

function verifyInitialState() {
  log("Verifying initial state.");
  ok(manager instanceof MozMobileMessageManager,
     "manager is instance of " + manager.constructor);
  // Ensure test is starting clean with no existing sms messages
  deleteAllMsgs(simulateIncomingSms);
}

function deleteAllMsgs(nextFunction) {
  // Check for any existing SMS messages, if any are found delete them
  let msgList = new Array();
  let filter = new MozSmsFilter;

  let cursor = manager.getMessages(filter, false);
  ok(cursor instanceof DOMCursor,
      "cursor is instanceof " + cursor.constructor);

  cursor.onsuccess = function(event) {
    // Check if message was found
    if (cursor.result) {
      msgList.push(cursor.result.id);
      // Now get next message in the list
      cursor.continue();
    } else {
      // No (more) messages found
      if (msgList.length) {
        log("Found " + msgList.length + " SMS messages to delete.");
        deleteMsgs(msgList, nextFunction);
      } else {
        log("No SMS messages found.");
        nextFunction();
      }
    }
  };

  cursor.onerror = function(event) {
    log("Received 'onerror' event.");
    ok(event.target.error, "domerror obj");
    log("manager.getMessages error: " + event.target.error.name);
    ok(false,"Could not get SMS messages");
    cleanUp();
  };
}

function deleteMsgs(msgList, nextFunction) {
  // Delete the SMS messages specified in the given list
  let smsId = msgList.shift();

  log("Deleting SMS (id: " + smsId + ").");
  let request = manager.delete(smsId);
  ok(request instanceof DOMRequest,
      "request is instanceof " + request.constructor);

  request.onsuccess = function(event) {
    log("Received 'onsuccess' smsrequest event.");
    if (event.target.result) {
      // Message deleted, continue until none are left
      if (msgList.length) {
        deleteMsgs(msgList, nextFunction);
      } else {
        log("Finished deleting SMS messages.");
        nextFunction();
      }
    } else {
      log("SMS delete failed.");
      ok(false,"manager.delete request returned false");
      cleanUp();
    }
  };

  request.onerror = function(event) {
    log("Received 'onerror' smsrequest event.");
    ok(event.target.error, "domerror obj");
    ok(false, "manager.delete request returned unexpected error: "
        + event.target.error.name );
    cleanUp();
  };
}

function simulateIncomingSms(remoteNumber) {
  // Simulate incoming SMS messages from specified (or default) remote number
  let text = "Incoming SMS number " + (smsList.length + 1);
  remoteNumber = typeof remoteNumber !== 'undefined'
    ? remoteNumber : defaultRemoteNumber;

  log("Simulating incoming SMS number " + (smsList.length + 1) + " of "
      + numberMsgs + ".");

  // Simulate incoming sms sent from remoteNumber to our emulator
  rcvdEmulatorCallback = false;
  runEmulatorCmd("sms send " + remoteNumber + " " + text, function(result) {
    is(result[0], "OK", "emulator callback");
    rcvdEmulatorCallback = true;
  });
}

manager.onreceived = function onreceived(event) {
  // Callback for incoming SMS
  log("Received 'onreceived' sms event.");
  let incomingSms = event.message;
  log("Received SMS (id: " + incomingSms.id + ").");

  smsList.push(incomingSms);

  // Wait for emulator to catch up before continuing
  waitFor(nextRep,function() {
    return(rcvdEmulatorCallback);
  });
};

function nextRep() {
  // Keep simulating incoming messages until have received specified number
  if (smsList.length < numberMsgs) {
    // Have every other sms be from different number, so filter won't find all
    if (smsList.length % 2) {
      simulateIncomingSms("+15559990000");
    } else {
      simulateIncomingSms();
    }
  } else {
    getMsgs();
  }
}

function getMsgs() {
  // Set the filter and test it via getMessages
  var filter = new MozSmsFilter();
  let foundSmsList = new Array();

  // Going to filter for one number only, so set our expected SMS array
  smsList = smsList.filter(function(i) {
    return i.sender != defaultRemoteNumber ? false: true;
  });

  // Set filter for default remote number
  filter.numbers = new Array(defaultRemoteNumber);

  log("Getting the SMS messages from sender " + defaultRemoteNumber + ".");
  let cursor = manager.getMessages(filter, false);
  ok(cursor instanceof DOMCursor,
      "cursor is instanceof " + cursor.constructor);

  cursor.onsuccess = function(event) {
    log("Received 'onsuccess' event.");

    if (cursor.result) {
      // Another message found
      log("Got SMS (id: " + cursor.result.id + ").");
      // Store found message
      foundSmsList.push(cursor.result);
      // Now get next message in the list
      cursor.continue();
    } else {
      // No more messages; ensure correct number of SMS messages were found
      if (foundSmsList.length == smsList.length) {
        log("SMS getMessages returned " + foundSmsList.length +
            " messages as expected.");
        verifyFoundMsgs(foundSmsList);
      } else {
        log("SMS getMessages returned " + foundSmsList.length +
            " messages, but expected " + smsList.length + ".");
        ok(false, "Incorrect number of messages returned by manager.getMessages");
        deleteAllMsgs(cleanUp);
      }
    }
  };

  cursor.onerror = function(event) {
    log("Received 'onerror' event.");
    ok(event.target.error, "domerror obj");
    log("manager.getMessages error: " + event.target.error.name);
    ok(false,"Could not get SMS messages");
    cleanUp();
  };
}

function verifyFoundMsgs(foundSmsList) {
  // Verify the SMS messages returned by getMessages are the correct ones
  for (var x = 0; x < foundSmsList.length; x++) {
    is(foundSmsList[x].id, smsList[x].id, "id");
    is(foundSmsList[x].sender, smsList[x].sender, "number");
  }
  deleteAllMsgs(cleanUp);
}

function cleanUp() {
  manager.onreceived = null;
  SpecialPowers.removePermission("sms", document);
  SpecialPowers.clearUserPref("dom.sms.enabled");
  finish();
}

// Start the test
verifyInitialState();
