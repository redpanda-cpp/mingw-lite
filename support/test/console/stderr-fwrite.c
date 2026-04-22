#include <stdio.h>
#include <string.h>

#include "util.h"

int main(void) {
  {
    const char *s = "天地玄黄\n";
    size_t len = strlen(s);

    fwrite(s, 1, len, stderr);
  }

  {
    const char *s =
        "天地玄黄，宇宙洪荒；日月盈昃，辰宿列张；寒来暑往，秋收冬藏；"
        "闰馀成岁，律吕调阳；雲腾致雨，露结为霜；金生丽水，玉出崑冈；"
        "剑号巨阙，珠称夜光；果珍李柰，菜重芥姜；海咸河淡，鳞潜羽翔。"
        "龙师火帝，鸟官人皇；始制文字，乃服衣裳；推位让国，有虞陶唐；"
        "吊民伐罪，周發殷汤；坐朝问道，垂拱平章；爱育黎首，臣伏戎羌；"
        "遐迩一体，率宾归王；鸣凤在竹，白驹食场；化被草木，赖及万方。\n";
    size_t len = strlen(s);

    fwrite(s, 1, len, stderr);
  }

  return 0;
}
