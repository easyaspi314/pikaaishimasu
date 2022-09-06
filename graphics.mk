BACKGROUNDS := GameOver LevelClear Level1 Level2 Level3 LevelName
CONVERTED_BACKGROUNDS := $(add)

GRAPHICS := \
	graphics/title_screen/PAL_TitleScreenName.pal \
	graphics/title_screen/IMG_TitleScreenName.bin \
	graphics/title_screen/IMG_TitleScreenGrass.bin \
	graphics/title_screen/IMG_TitleScreenCloud.bin \
	graphics/title_screen/IMG_TitleScreenTrees.bin \
	graphics/backgrounds/PAL_Disclaimer.pal \
	graphics/backgrounds/IMG_Disclaimer.bin \
	graphics/backgrounds/IMG_Copyright.bin \
	graphics/backgrounds/IMG_GameOver.bin \
	graphics/backgrounds/PAL_GameOver.pal \
	graphics/backgrounds/PAL_LevelClear.bin \
	graphics/backgrounds/IMG_LevelClear.pal \
	graphics/backgrounds/PAL_LevelName.bin \
	graphics/backgrounds/IMG_LevelName.pal \
	graphics/backgrounds/IMG_Level1.bin \
	graphics/backgrounds/PAL_Level1.pal \
	graphics/backgrounds/IMG_Level2.bin \
	graphics/backgrounds/PAL_Level2.pal \
	graphics/backgrounds/IMG_Level3.bin \
	graphics/backgrounds/PAL_Level3.pal \
	graphics/sprites/IMG_Balls.bin \
	graphics/sprites/IMG_Font.bin \
	graphics/sprites/IMG_Pikachu.bin \
	graphics/sprites/PAL_Pikachu.pal

C_DATA := $(patsubst %.bin,%.c,$(patsubst %.pal,%.c,$(GRAPHICS)))
%.c: %.bin $(BIN2C)
	$(BIN2C) $< $@ 32

%.c: %.pal $(BIN2C)
	$(BIN2C) $< $@ 8

IMG_%.bin: %.bmp $(BMP2BIN)
	$(BMP2BIN) $< $@

PAL_%.pal: %.bmp $(BMP2BIN)
	$(BMP2BIN) $< $@

