<!-- Generated by documentation.js. Update this documentation by updating the source code. -->

# count

Count the number of items in an osm.pbf file

**Parameters**

-   `path` **[String](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String)** to input pbf to read
-   `callback` **[Function](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/function)** called when work is done, passed `err` or `null` as first arg, and an object as second arg with counts of osm features

**Examples**

```javascript
var osmium = require('osmium_count');
osmium.count("input.osm.pbf", function(err, results) {
  if (err) throw err;
  console.log(results);
});
```
