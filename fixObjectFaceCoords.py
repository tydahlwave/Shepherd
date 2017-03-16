import sys

def parseInputFile(inputFile, vertexOffset, textureOffset, normalOffset):
   with open(inputFile, 'r') as file:
      for line in file:
         line = line.strip()

         if line and line[0] == 'f':
            vertices = line.split(' ')
            print("f ", end='')
            for i in range(1, len(vertices)):
               vertex = vertices[i]
               coords = vertex.split('/')
               print("{0}/{1}/{2} ".format((int(coords[0]) - vertexOffset), (int(coords[1]) - textureOffset), (int(coords[2]) - normalOffset)), end='')
            print()
         else:
            print(line)


def main(argv):
   parseInputFile(argv[1], int(argv[2]), int(argv[3]), int(argv[4]))

main(sys.argv)
