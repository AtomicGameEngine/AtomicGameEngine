

for css, first export default template to atomictemplate.xml

```mdoc export-html --default-template > atomictemplate.xsl```

Then modify it, for example in ```span.NotEntered``` add property "```display: none;```" to hide red "to be added"

This xsl needs to be checked into source control, to maintain mods

// process to xml

```mdoc update -i AtomicNET.XML AtomicNET.dll -o xmldocs```

// convert to html, with atomic template

```mdoc export-html --template=atomictemplate.xsl -o html ./htmldocs```
