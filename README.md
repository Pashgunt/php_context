# 🧩 PHP Extension Context
![PHP version](https://img.shields.io/badge/php-%3E=8.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen)

> **Context** —  is a native PHP extension that implements an *execution context* with timeout and cancellation  
> By analogy with [`context.Context`](https://pkg.go.dev/context) from Go: helps to limit the running time of a script or part of the code.

---

## 🚀 Install

### 📦 Source code build

```bash
git clone https://github.com/yourname/php_context.git
cd php_context
mkdir build && cd build
cmake ..
make
sudo make install
```

Add to `php.ini`:
```ini
extension=context.so
```

### 🐘 (Future) Install from PECL

```bash
pecl install context
```

---

## ✏️ Examples

```php
<?php
$ctx = new Context(3); // 3 seconds timout

try {
    $result = $ctx->run(function() use ($ctx) {
        while (true) {
            if ($ctx->isCancelled()) {
                throw new Exception("Interrupted by timeout!");
            }

            usleep(100_000); // long-term operation emulation
        }

        return "done";
    });
    echo "Success: $result\n";
} catch (Exception $e) {
    echo "Error: " . $e->getMessage() . "\n";
}
```

After 3 seconds we get:

```
Error: Interrupted by timeout!
```

---

## ⚙️ How it works

- The context is created with a timeout: `new Context(3)`.
- Run the function in the context of: `$ctx->run(...)`.
- Inside you can check `$ctx->isCancelled()'.
- When the time expires, the context is marked as cancelled.
- You need to *check* the cancellation yourself and throw an exception or stop the loop.

---

## 🧰 Real-world application examples

### ✅ Processing a large array

```php
$ctx = new Context(5);

$ctx->run(function() use ($ctx) {
    foreach (range(1, 10_000_000) as $i) {
        if ($ctx->isCancelled()) {
            throw new Exception("Aborted on $i element");
        }
        // data processing
    }
    
    return "Done!";
});
```

---

### ✅ Complex algorithm (for example, pathfinding in a graph)

```php
$ctx = new Context(2);

$ctx->run(function() use ($ctx) {
    $result = null;
    
    while (!$result) {
        if ($ctx->isCancelled()) {
            throw new Exception("The search was interrupted");
        }
        // data processing
    }
    
    return $result;
});
```

---

## 📌 It is important to understand

### ✅ Where it helps:
- CLI scripts, background tasks, daemons.
- Huge cycles that need to be interrupted.
- Complex algorithms where you can manually check `$ctx->isCancelled()'.

## ⚠️ Where it does not help:
- PHP does not know how to interrupt blocking calls 'sleep()'` 'usleep()`.
- Does not interrupt the network request `curl_exec` or `file_get_contents' (use timeouts at the cURL/PDO level).
- Does not cancel database calls that are already running.

The context gives you control at the level of your code, not at the level of the PHP interpreter.
