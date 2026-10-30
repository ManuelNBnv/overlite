# Overlite (SDL3)

Versión gráfica de Overlite hecha con **SDL3 + SDL3_ttf**, sin librerías de widgets:
los botones, campos de texto y el foco se dibujan y se manejan a mano en `src/ui.cpp`.

## Estructura

| Archivo | Qué hace |
| --- | --- |
| `src/logic.h/.cpp` | Lógica sin interfaz: calculadora, Luhn, generador, promedio, usuarios |
| `src/ui.h/.cpp` | Mini toolkit: texto con caché, botones, campos de texto (foco, Tab, pegar) |
| `src/main.cpp` | Ventana, bucle principal y una función por pantalla |
| `tests/test_logic.cpp` | Pruebas de la lógica |

## Compilar

Necesitas SDL3 y SDL3_ttf (por ejemplo `vcpkg install sdl3 sdl3-ttf`, `brew install sdl3 sdl3_ttf`,
o el paquete de tu distribución; si no los trae, compílalos desde https://github.com/libsdl-org).

```
cmake -B build
cmake --build build
./build/overlite        # ejecútalo desde la carpeta del proyecto
```

Pruebas de la lógica (no necesitan SDL): `cmake -B build -DOVERLITE_TESTS=ON && cmake --build build && ctest --test-dir build`

## Fuente

El programa busca `assets/font.ttf` y, si no está, fuentes comunes del sistema
(DejaVu, Liberation, Arial, Segoe UI). Si no encuentra ninguna, copia una `.ttf` a `assets/font.ttf`.

## Controles

Clic para elegir campo, `Tab` para pasar al siguiente, `Enter` para confirmar,
`Ctrl+V` (`Cmd+V` en Mac) para pegar.

## Datos

Los usuarios se guardan en `DATA/users.json` (una línea JSON por usuario, mismo formato de la versión
de consola). **Las contraseñas están en texto plano**, igual que antes: sirve para practicar,
no para guardar contraseñas reales.
