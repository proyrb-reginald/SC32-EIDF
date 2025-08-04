import sys
import os
from PIL import Image

def check_jpeg_type(image_path):
    """
    检测JPEG图片类型（渐进式或基线式）
    """
    try:
        with Image.open(image_path) as img:
            if img.format == 'JPEG':
                # 检查是否为渐进式
                if img.info.get('progressive', False):
                    return "渐进式JPEG"
                else:
                    return "基线JPEG"
            else:
                return f"非JPEG格式（实际格式：{img.format}）"
    except Exception as e:
        return f"错误：无法打开文件 - {str(e)}"

def main():
    # 检查命令行参数
    if len(sys.argv) < 2:
        print("使用方法：")
        print("  单个文件：python check_jpeg.py <图片文件路径>")
        print("  多个文件：python check_jpeg.py <文件1> <文件2> ...")
        print("示例：")
        print("  python check_jpeg.py image.jpg")
        print("  python check_jpeg.py *.jpg")
        sys.exit(1)
    
    # 处理所有传入的文件
    files = sys.argv[1:]
    
    if len(files) == 1:
        # 单个文件
        image_path = files[0]
        result = check_jpeg_type(image_path)
        print(f"文件：{image_path}")
        print(f"类型：{result}")
    else:
        # 多个文件
        print(f"共检测 {len(files)} 个文件：")
        print("-" * 50)
        for image_path in files:
            result = check_jpeg_type(image_path)
            print(f"{image_path:<30} | {result}")

if __name__ == "__main__":
    main()