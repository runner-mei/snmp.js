var util = require('util'),
    dgram = require("dgram");

var assertBuffer = function(assert, actual, expected) {
          assert.ok(Buffer.isBuffer(actual), "This isn`t a Buffer.");
          if(!Buffer.isBuffer(expected)) {
                assert.equal(actual.toString(), expected);
          } else {
                assert.deepEqual(actual, expected);
          }
        };


        module.exports = {
            'require session': function (beforeExit, assert) {
                var snmp = require('snmp');
                var session = new snmp.Native.Session();
            },
            'test session attributes': function (beforeExit, assert) {
                var snmp = require('snmp');
                var session = new snmp.Native.Session();

                session.version = 1;
                session.retries = 2;
                session.timeout = 3;
                session.flags = 4;
                session.peername = "5";
                session.remote_port = 6;
                session.localname = "7";
                session.local_port = 8;

                session.sessid = 9;
                session.community = "aa";
                session.rcvMsgMaxSize = 10;
                session.sndMsgMaxSize = 11;

                session.isAuthoritative = 12;
                session.contextEngineID = "13";
                session.engineBoots = 14;
                session.engineTime = 15;
                session.contextName = "16";
                session.securityEngineID = "17";
                session.securityName = "18";

                session.securityAuthProto = [1, 23, 4, 5, 19];
                session.securityAuthKey = "20";
                session.securityAuthLocalKey = "21";
                session.securityPrivProto = [1, 23, 4, 5, 22];
                session.securityPrivKey = "23";
                session.securityPrivLocalKey = "24";
                session.securityModel = 25;
                session.securityLevel = 26;
                session.paramName = "27";



                assert.equal(session.version, 1);
                assert.equal(session.retries, 2);
                assert.equal(session.timeout, 3);
                assert.equal(session.flags, 4);
                assert.equal(session.peername, "5");
                assert.equal(session.remote_port, 6);
                assert.equal(session.localname, "7");
                assert.equal(session.local_port, 8);

                assert.equal(session.sessid, 9);
                assertBuffer(assert, session.community, "aa");
                assert.equal(session.rcvMsgMaxSize, 10);
                assert.equal(session.sndMsgMaxSize, 11);

                assert.equal(session.isAuthoritative, 12);
                assertBuffer(assert, session.contextEngineID, "13");
                assert.equal(session.engineBoots, 14);
                assert.equal(session.engineTime, 15);
                assert.equal(session.contextName, "16");
                assertBuffer(assert, session.securityEngineID, "17");
                assert.equal(session.securityName, "18");

                assert.deepEqual(session.securityAuthProto, [1, 23, 4, 5, 19]);
                assertBuffer(assert, session.securityAuthKey, "20");
                assertBuffer(assert, session.securityAuthLocalKey, "21");
                assert.deepEqual(session.securityPrivProto, [1, 23, 4, 5, 22]);
                assertBuffer(assert, session.securityPrivKey, "23");
                assertBuffer(assert, session.securityPrivLocalKey, "24");
                assert.equal(session.securityModel, 25);
                assert.equal(session.securityLevel, 26);
                assert.equal(session.paramName, "27");


                session.securityAuthKey = "1234567890123456789012345678901234567890"
                assertBuffer(assert, session.securityAuthKey, "12345678901234567890123456789012");


                session.securityPrivKey = "1234567890123456789012345678901234567890"
                assertBuffer(assert, session.securityPrivKey, "12345678901234567890123456789012");

            },
            'test get': function (beforeExit, assert) {
                var snmp = require('snmp');
                var session = snmp.createSession();
                var rs;
                var pdu;

                session.peername = "127.0.0.1";
                session.openSync();

                pdu = snmp.createPdu("get");
                pdu.community = "public";
                //snmpget -v 2c -c public 127.0.0.1 system.sysDescr.0
                pdu.version = snmp.SNMP_VERSION.v2c;
                //pdu.variableBindings.add("system.sysDescr", snmp.DATA_TYPE.ASN_NULL, null);
                pdu.variableBindings.add("1.3.6.1.2.1.1.1.0", snmp.DATA_TYPE.ASN_NULL, null);

                rs = session.sendSync(pdu);
                assert.notEqual(null, rs);
                session.close();
            },

            'test get timeout': function (beforeExit, assert) {
                var snmp = require('snmp');
                var session = snmp.createSession();
                var rs;
                var pdu;
                var server;
                var recv;

                session.peername = "127.0.0.1:6789";
                session.openSync();
                session.timeout = 1 * 1000000;

                pdu = snmp.createPdu("get");
                pdu.community = "public";
                //snmpget -v 2c -c public 127.0.0.1 system.sysDescr.0
                pdu.version = snmp.SNMP_VERSION.v2c;
                //pdu.variableBindings.add("system.sysDescr", snmp.DATA_TYPE.ASN_NULL, null);
                pdu.variableBindings.add("1.3.6.1.2.1.1.1.0", snmp.DATA_TYPE.ASN_NULL, null);


                server = dgram.createSocket("udp4");

                server.on("message", function (msg, rinfo) {
                    recv = msg;
                });

                server.bind(6789);

                try {
                    rs = session.sendSync(pdu);
                    assert.fail();
                } catch (e) {
                }
                session.close();
                server.close();
            },

            'test send error': function (beforeExit, assert) {
                var snmp = require('snmp');
                var session = snmp.createSession();
                var rs;
                session.peername = "127.0.0.1:162";
                session.openSync();

                var pdu = snmp.createPdu("get");
                pdu.community = "public";
                //snmpget -v 2c -c public 127.0.0.1 system.sysDescr.0
                pdu.version = snmp.SNMP_VERSION.v2c;
                //pdu.variableBindings.add("system.sysDescr", snmp.DATA_TYPE.ASN_NULL, null);
                pdu.variableBindings.add("1.3.6.1.2.1.1.1.0", snmp.DATA_TYPE.ASN_NULL, null);

                try {
                    rs = session.sendSync(pdu);
                    assert.fail();
                } catch (e) {
                }
                session.close();
            },

            'test async get': function (beforeExit, assert) {
                var snmp = require('snmp');
                var session = snmp.createSession();
                var rs;
                var pdu;
                session.peername = "mei:127.0.0.1:161";
                session.open();

                pdu = snmp.createPdu("get");
                pdu.community = "public";
                //snmpget -v 2c -c public 127.0.0.1 system.sysDescr.0
                pdu.version = snmp.SNMP_VERSION.v2c;
                //pdu.variableBindings.add("system.sysDescr", snmp.DATA_TYPE.ASN_NULL, null);
                pdu.variableBindings.add("1.3.6.1.2.1.1.1.0", snmp.DATA_TYPE.ASN_NULL, null);

                session.send(pdu, function (err, request, response) {
                    rs = response;
                    session.close();
                });

                beforeExit(function () {
                    assert.notEqual(null, rs);
                });
            },

            'test async get timeout': function (beforeExit, assert) {
                var snmp = require('snmp');
                var session = snmp.createSession();
                var rs;
                var pdu;
                var server;
                var recv;
                var error;

                session.peername = "mei:127.0.0.1:6789";
                session.open();

                pdu = snmp.createPdu("get");
                pdu.community = "public";
                //snmpget -v 2c -c public 127.0.0.1 system.sysDescr.0
                pdu.version = snmp.SNMP_VERSION.v2c;
                //pdu.variableBindings.add("system.sysDescr", snmp.DATA_TYPE.ASN_NULL, null);
                pdu.variableBindings.add("1.3.6.1.2.1.1.1.0", snmp.DATA_TYPE.ASN_NULL, null);

                server = dgram.createSocket("udp4");
                server.on("message", function (msg, rinfo) {
                    recv = msg;
                    server.close();
                });

                server.on('error', function (exception) {
                    server.close();
                });

                server.bind(6789);

                session.send(pdu, function (err, request, response) {
                    try {
                        error = err;
                        rs = response;
                        session.close();

                        //console.log(util.inspect(rs.toJS(), true, null));
                    } catch (e) {
                        console.log(e);
                    }
                });

                beforeExit(function () {
                    assert.equal("timeout", error);
                    assert.notEqual(null, rs);
                });
            }

        };