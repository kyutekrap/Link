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

### 🔤 Naming Convention
- **Functions:** Lower snake case 🐍
- **Variables:** Upper camel case 🐫

---

### 🧰 Built-in Functions  
- `#flow` - Inherited from PyLink's Flow
- `#step` - Inherited from PyLink's Step
- `#data` - Defines file as data
- `@debug()` - Inherited from PyLink's Debug
- `@global()` - Define global constant variable in Flow
- `@import()` - Import Steps to Flow
- `info()` – Inherited from PyLink's Debugger  
- `warning()` – Inherited from PyLink's Debugger  
- `error()` – Inherited from PyLink's Debugger
- `die()` - Inherited from PyLink's System.Die
- `depends()` - Works like a switch statement
- `while()` - Works like a while statement with nested if's
- `eq()` - Comparator for =
- `gt()` - Comparator for >
- `lt()` - Comparator for <
- `gte()` - Comparator for >=
- `lte()` - Comparator for <=

### 🗑️ Removed Functions
- `@param()` - Set parameters for function
- `@set()` - Set global variable in Flow
- `@get()` - Get global variable in Step
- `SetStep()` - Inherited from PyLink's SetStep
- `GetStep()` - Inherited from PyLink's GetStep
- `SetFlow()` - Inherited from PyLink's SetFlow
- `GetFlow()` - Inherited from PyLink's GetFlow
- `Decision()` - Inherited from PyLink's Decision
- `RecursiveFlow()` - Inherited from PyLink's RecursiveFlow
- `RecursiveStep()` - Inherited from PyLink's RecursiveStep

---

### 🔧 In-Line Operators
- ["A", "B", "C"] - Creates a list of strings
- [100, 200, 300] - Creates a list of integers
- [100.0, 200.0, 300.0] - Creates a list of doubles
- {["A", "B", "C"], [100, 200, 300]} - Creates a map of two lists

---

### 🔢 Supported Data Types
- String
- Integer
- Double
- String List
- Integer List
- Double List
- String Map
- Integer Map
- Double Map

---

>🔹 **Note**  
Link has external support for C libraries that you can use with the `@import` function.