from pypdf import PdfWriter


merger = PdfWriter()

merger.append("Train 4 Compiled Drawings 19112025.pdf")     
merger.append("210587.pdf")   


merger.write("Train 4 Compiled Drawings 19112025-210587.pdf")
merger.close()

print("Completed!")