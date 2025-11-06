# SmartPark
SmartPark é um jogo educativo em C que simula um parquinho inteligente, combinando estruturas de dados (lista circular, fila e pilha) e inteligência artificial para gerenciar eventos e emoções das crianças (NPCs) de forma interativa e divertida.

SmartPark é um jogo educativo em C que simula o gerenciamento de um parquinho
inteligente. Ele combina estruturas de dados (lista circular, fila e pilha),
algoritmos de ordenação e um módulo de inteligência artificial para controlar o
humor dos visitantes.

## Compilando

Para compilar a versão padrão baseada em terminal, execute:

```bash
make
```

### Executando a versão com GUI (`make gui`)

A versão gráfica utiliza Raylib e, opcionalmente, a API C do TensorFlow para a
IA. Em um macOS (ambiente alvo do projeto) instale as dependências com o
[Homebrew](https://brew.sh/):

```bash
brew install raylib
brew install libtensorflow
```

Após a instalação, confirme que os diretórios padrão do Homebrew (`/opt/homebrew/include`
e `/opt/homebrew/lib`) estejam visíveis para o compilador. Caso utilize caminhos
personalizados, defina previamente `CFLAGS` e `LDFLAGS`:

```bash
export CFLAGS="-I/caminho/para/includes"
export LDFLAGS="-L/caminho/para/libs"
```

O subsistema de IA procura o modelo salvo em
`/Users/marianacorreia/parquinho_inteligente/model` e a biblioteca TensorFlow em
`/opt/homebrew/lib/libtensorflow.dylib`. Esses caminhos podem ser ajustados com
as seguintes variáveis de ambiente:

```bash
export SMARTPARK_TF_LIB="/caminho/para/libtensorflow.dylib"
export SMARTPARK_TF_MODEL_DIR="/caminho/para/modelo"
export SMARTPARK_TF_INPUT_OP="nome_da_op_entrada"
export SMARTPARK_TF_OUTPUT_OP="nome_da_op_saida"
```

Com as dependências instaladas e as variáveis configuradas (se necessário),
execute:

```bash
make gui
```

O binário `smartpark_gui` será gerado na raiz do projeto.

## Execução

Após a compilação, rode a versão desejada:

```bash
./smartpark_cli   # modo texto
./smartpark_gui   # modo gráfico
```

## Dependências principais

- [Raylib](https://www.raylib.com/)
- [TensorFlow C API](https://www.tensorflow.org/install/lang_c) (opcional, para inferência real)

## Estrutura de dados e algoritmos

- Lista circular para percorrer os setores do parque.
- Fila de eventos ordenada por prioridade para atender ocorrências críticas.
- Pilha para desfazer ações (snapshots).
- QuickSort para ordenar o ranking dos jogadores.

## IA

O módulo `src/ia.c` tenta inicializar o TensorFlow no macOS e carregar um
SavedModel. Caso o runtime não esteja disponível, o jogo utiliza uma heurística
com amortecimento para calcular a paciência dos NPCs.
