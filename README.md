# 🎮 SmartPark

**Autora:** Mariana Medeiros Barros Correia

---

## 🧠 Objetivo

Administrar o parquinho e manter as crianças felizes resolvendo eventos que aparecem em cada brinquedo (**balanço**, **sorvete** e **escorrega**).

Cada evento atendido rapidamente gera **pontos**; atrasos acumulam **penalidades**.

---

### 🌀 Fluxo geral

1. Ao iniciar o executável, há **3 opções**: *Jogar*, *Ver ranking*, e *Sair*.
   → Escolha **“Jogar”** com o mouse.

2. Informe seu nome (até **15 caracteres**) e pressione `Enter`.

3. O relógio começa a contar **(01:30 minutos regressivos)**.
   Use o personagem para circular pelos setores e resolver os eventos antes que a fila de crianças fique impaciente.

4. Quando o tempo acaba ou você pressiona `Q`, a pontuação é **salva no ranking**.

---

## 🎮 Controles principais

| Tecla                | Função                                             |
| -------------------- | -------------------------------------------------- |
| `Mouse` ou `W` / `S` | Navegam entre as opções do menu                    |
| `Enter`              | Confirma a opção selecionada e confirma o nome     |
| `Esc`                | Fecha o jogo                                       |
| `A` / `W`            | Move o personagem para o setor anterior            |
| `D` / `S`            | Move o personagem para o próximo setor             |
| `H`                  | Resolve o evento do setor atual e concede pontos   |
| `U`                  | Desfaz a última ação (usa o topo da pilha de undo) |
| `Q`                  | Termina a partida e registra o score atual         |

---

### 💡 Outras observações

* Os **eventos são gerados dinamicamente** com base no humor médio das crianças (IA).
* Deixar eventos em espera por muito tempo pode gerar **penalidades leves** ou **severas**.
* A **fila de eventos** é exibida na lateral da tela.
* Use o mapa para planejar o caminho mais rápido até o setor com problemas.

---

## 💻 Como executar o jogo

### 🪟 Windows (MSYS2 MinGW 64-bit)

1. **Baixe e instale o [MSYS2](https://www.msys2.org/)**.

2. **Abra o terminal** “MSYS2 MinGW 64-bit” (não o MSYS padrão).

3. **Atualize os pacotes base:**

   ```bash
   pacman -Syu
   # se pedir, feche e abra o terminal novamente e rode o comando outra vez
   ```

4. **Instale as dependências:**

   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-raylib
   ```

5. **Entre na pasta do projeto:**

   ```bash
   cd /c/SmartPark
   ```

6. **Compile o jogo:**

   ```bash
   mingw32-make gui
   ```

   O executável será gerado em:

   ```
   build/jogo_gui.exe
   ```

7. **Execute o jogo:**

   ```bash
   ./build/jogo_gui.exe
   ```

---

### 🍎 macOS (Apple Silicon ou Intel)

1. **Instale as ferramentas de compilação (se necessário):**

   ```bash
   xcode-select --install
   ```

2. **Instale o Homebrew (se ainda não tiver):**

   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Instale as bibliotecas necessárias:**

   ```bash
   brew install raylib
   # Opcional (IA real com TensorFlow)
   brew install libtensorflow
   ```

4. **Entre na pasta do projeto:**

   ```bash
   cd /Users/seuusuario/SmartPark
   ```

5. **Compile o jogo:**

   ```bash
   make gui
   ```

   O executável será criado em:

   ```
   build/jogo_gui
   ```

6. **Execute o jogo:**

   ```bash
   ./build/jogo_gui
   ```

---

✅ **Pronto!**
O SmartPark abrirá com o menu principal.
Use **W/S** ou o **mouse** para navegar e **Enter** para selecionar.
Divirta-se administrando o parquinho! 🎠🍦🛝

