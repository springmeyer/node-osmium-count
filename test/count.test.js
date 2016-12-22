var test = require('tape');
var osmium = require('../');

test('counting items in pbf file', function(t) {
  osmium.count(__dirname + '/data/vancouver_canada.osm.pbf',function(err,results) {
    if (err) throw err;
    t.equals(results.ways, 154488, 'ways');
    t.equals(results.nodes, 800769, 'nodes');
    t.equals(results.relations, 924, 'relations');
    t.end();
  });
});

// error handling
test('file does not exist', function(t) {
  osmium.count(__dirname + '/data/does not exist',function(err,results) {
    t.ok(err);
    t.end();
  });
});

test('invalid args', function(t) {
  osmium.count(null,function(err,results) {
    t.ok(err);
    t.end();
  });
});

test('invalid args - no callback', function(t) {
  try  {
     osmium.count('path');
  } catch (err) {
     t.end();
  }
});
