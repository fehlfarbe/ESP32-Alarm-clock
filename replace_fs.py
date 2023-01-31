Import("env")
print("Replace MKSPIFFSTOOL with mklittlefs")
env.Replace (MKSPIFFSTOOL = "./update_fs.sh")
# was renamed in newer pio versions
env.Replace (MKFSTOOL = "./update_fs.sh")

# debug
# print("\n".join([f"{k}: {v}" for k, v in env.items()]))