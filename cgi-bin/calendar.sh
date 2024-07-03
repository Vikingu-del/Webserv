#!/bin/bash

# Output HTTP headers
printf "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"

# Output HTML content
cat <<EOF
<h3>Calendar:</h3>
<pre>$(cal)</pre>

<h3>Date: </h3>
<pre>$(date)
$(date -u)</pre>

<h4>Days have passed since the beginning of the year: </h4>
<pre>$(date +%j)</pre>
EOF