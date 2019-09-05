if [ ! -d "Website" ]; then
    mkdir -p Website/{static/{cs,js},templates/html,imgs/{gif,jpg,png}}
    touch Website/index.html
    touch Website/menu.html
    touch Website/second.html
    touch Website/third.html
    touch Website/static/js/menu.js
    touch Website/templates/html/menu.htm
    touch Website/templates/some.html
    touch Website/imgs/png/a
    touch Website/imgs/png/b
    touch Website/imgs/png/c
    touch Website/imgs/png/d
    touch Website/imgs/png/e
    touch Website/imgs/jpg/jp1
    touch Website/imgs/jpg/jp2
    touch Website/imgs/gif/gi1
    touch Website/imgs/gif/gi2
    touch Website/imgs/gif/gi3
fi

test.out
