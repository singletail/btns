# Configuration

## Set target

```bash
idf.py set-target esp32c6
```

## full clean

```bash
idf.py clean
```

## sdkconfig defaults

- Enable/disable options in sdkconfig.defaults
- Run idf.py menuconfig (probably twice)
- Verify settings in sdkconfig

## config.h

- Pick/create:
    - ID
    - Device ID
    - Device name
    - Device model

## define.h

- Enable modules for your ID
- Set pins

## include.h

- Verify all exist
- Verify they match definitions
- If #ifdef not updated:
    - idf.py clean
    - idf.py set-target <target>
    - idf.py reconfigure
    - idf.py update-dependencies
    - idf.py menuconfig
    - idf.py build

## CMakeLists.txt (inside main component)

- Check REQUIRES
    - Add any missing, remove any unused
    - check build output for component list

