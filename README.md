# **Link**  
*A Superset of C*

---

### 🧠 Background  
Link is a migration from **PyLink**, a Python-based wrapper API. The aim is to move towards a more native C-based language.

### 🎯 Abstract
Link enforces a custom syntax unique to its language — different from PyLink — and transpiles it into standard C code.

### 🚀 Goal  
To create a lightweight, utility-focused language that blends the **simplicity of scripting** with the **power and performance of C**.

---

### 💻 Development Environment  
- **Platform:** Windows only  
- **IDE Support:** VS Code (Link Syntax)

---

### 🧰 Built-in Functions  
- `#Flow` - Inherited from PyLink's Flow
- `#Step` - Inherited from PyLink's Step
- `@debug()` - Inherited from PyLink's Debug
- `@set()` - Set global variable in Flow
- `@get()` - Get global variable in Step
- `@import()` - Import external functions
- `Info()` – Inherited from PyLink's Debugger  
- `Warning()` – Inherited from PyLink's Debugger  
- `Error()` – Inherited from PyLink's Debugger
- `Die()` - Inherited from PyLink's System.Die

### 🗑️ Removed Functions
- `@param()` - Set parameters for function
- `SetStep()` - Inherited from PyLink's SetStep
- `GetStep()` - Inherited from PyLink's GetStep
- `SetFlow()` - Inherited from PyLink's SetFlow
- `GetFlow()` - Inherited from PyLink's GetFlow