# 📁 Experiment: File Handling Basics


## 📂 File Creation Modes & `fopen()`

In C, you use the **`fopen()`** function to open or create files. It takes **two arguments**:

``c
FILE *fptr = fopen("filename", "mode");
### File Name and Mode in `fopen()`

- **`filename`** — the name or path of the file.
- **`mode`** — how you want to open the file. The common modes are:

| Mode  | Description                                  |
|-------|----------------------------------------------|
| `"r"` | Read mode — open existing file for reading   |
| `"w"` | Write mode — create new file or overwrite old|
| `"a"` | Append mode — write at file’s end or create if missing |

- If successful, `fopen()` returns a file pointer (`FILE *`).
- If it fails (e.g., file doesn’t exist for `"r"` mode), it returns `NULL`.

---

### 📖 Reading & Writing Files

- **`fscanf(FILE *stream, const char *format, ...)`**  
  Reads formatted input from a file (like `scanf` but from a file).

- **`fprintf(FILE *stream, const char *format, ...)`**  
  Writes formatted output to a file (like `printf` but to a file).

---

### 🎤 Console I/O Functions

- **`puts(const char *str)`**  
  Prints a whole string **with a newline** — great for quick messages.

- **`putchar(int char)`**  
  Prints **one character** at a time — useful for detailed output.

- **`getchar()`**  
  Reads **one character** from keyboard input.

- **`EOF` (End Of File)**  
  A special constant signaling **no more input** is available — often used to detect file or input stream ends.

