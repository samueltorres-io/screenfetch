
1. Carrega o arquivo *conf* para ler todos os frames, cores e outros recursos.

2. Carrega o *JSON do usuário* para pegar as preferências de tema, animação e dados que ele optou no json.

3. Mapeia a preferência do JSON para os dados do conf:

    - "theme.foreground": "purple", ele pega o hash code #800080 no conf.

    - "animation.donut;status": true, ele pega os frames de donut do conf e aplica a velocidade no JSON.

4. Executa a anaimação no terminal usando os frames, cores e infos que o user definiu.