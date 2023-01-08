class PropertyGetter:
	def __init__(self, name, is_foreign, is_static, docs=""):
		self.name = name
		self.is_foreign = is_foreign
		self.is_static = is_static
		self.docs = docs
	
	def __repr__(self) -> str:
		return f"{self.name} {self.is_foreign} {self.is_static}"

class PropertySetter:
	def __init__(self, name, is_foreign, is_static, rhs, docs=""):
		self.name = name
		self.is_foreign = is_foreign
		self.is_static = is_static
		self.rhs = rhs
		self.docs = docs

	def __repr__(self) -> str:
		return f"{self.name} {self.is_foreign} {self.is_static}"

class SimpleMethod:
	def __init__(self, name, is_foreign, is_static, args, docs=""):
		self.name = name
		self.is_foreign = is_foreign
		self.is_static = is_static
		self.args = args
		self.docs = docs

	def __repr__(self) -> str:
		return f"{self.name} {self.is_foreign} {self.is_static}"

class Class:
	def __init__(self, name, is_foreign, docs=""):
		self.name = name
		self.is_foreign = is_foreign
		self.static_methods = []
		self.methods = []
		self.docs = docs
		self.last_item = None

	def add_method(self, method):
		self.methods.append(method)
		self.last_item = method

	def add_static_method(self, static):
		self.static_methods.append(static)
		self.last_item = static

class Module:
	def __init__(self):
		self.classes = []

	def add_class(self, klass: Class):
		self.classes.append(klass)

def method_from_signature(signature: str):
	is_foreign = signature.startswith("foreign ")
	is_static = "static " in signature

	raw_signature = signature
	if is_foreign:
		raw_signature = raw_signature[len("foreign "):]
	if is_static:
		raw_signature = raw_signature[len("static "):]

	if "=(" in raw_signature:
		left, right = raw_signature.split("=(")
		return PropertySetter(left, is_foreign, is_static, right[:-1])
	if "(" in raw_signature:
		left, right = raw_signature.split("(")
		return SimpleMethod(left, is_foreign, is_static, right[:-1].replace(' ', '').split(','))
	else:
		return PropertyGetter(raw_signature, is_foreign, is_static)

class WrenParser:
	def __init__(self):
		self.module = Module()
		self.docs = ""
	
	@staticmethod
	def indent_level(line):
		spaces = 0
		for c in line:
			if c == ' ':
				spaces += 1
			else:
				return spaces

	def klass(self, klass: Class):
		self.module.add_class(klass)
		self.flush(True)
	
	def method(self, method):
		self.module.classes[-1].add_method(method)
		self.flush(False)

	def static_method(self, method):
		self.module.classes[-1].add_static_method(method)

	def doc(self, stripped_line):
		self.docs += stripped_line[3:].strip() + ' '

	def flush(self, flush_class):
		if self.docs:
			self.docs = self.docs.strip()
			if flush_class:
				self.module.classes[-1].docs = self.docs
			else:
				self.module.classes[-1].last_item.docs = self.docs
			self.docs = ""

	def parse(self, source: str):
		for line in source.splitlines():
			line = line.replace('\t', ' ' * 4)
			stripped_line = line.strip()
			indent = WrenParser.indent_level(line)

			# classes
			if indent == 0:
				if stripped_line.startswith("foreign class"):
					self.klass(Class(stripped_line[len("foreign class"):].replace('{', '').strip(), True))
				elif stripped_line.startswith("class"):
					self.klass(Class(stripped_line[len("class"):].replace('{', '').strip(), False))
				elif stripped_line.startswith("///"):
					self.doc(stripped_line)
				# elif stripped_line.startswith("#"):
				# 	self.klass_attribute(stripped_line[1:])

			# methods
			elif indent == 4:
				if not stripped_line.startswith("//"):
					signature = ""
					# non-foreign
					if '{' in stripped_line:
						signature = stripped_line.split('{')[0].strip()
					# foreign
					elif stripped_line.startswith("foreign"):
						signature = stripped_line
					method = method_from_signature(signature)
					if method.is_static:
						self.static_method(method_from_signature(signature))
					else:
						self.method(method_from_signature(signature))
				elif stripped_line.startswith("///"):
					self.doc(stripped_line)
				# elif stripped_line.startswith("#"):
				# 	self.method_attribute(stripped_line[1:])


import sys
source = open("OverEngine/src/Wren/entity.wren").read()

parser = WrenParser()
parser.parse(source)
print("Done")
 