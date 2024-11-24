
## 1. Recherche dans un CV de keywords

```shell
cat demo/cv.txt | grep -Eo "Python" | wc -l
```


## 2. Recherche sur internet

```shell
sudo apt install html2text  # Posséder ceci avant
```

```shell
curl "https://www.odoo.com/fr_FR" | html2text > website.txt
```

