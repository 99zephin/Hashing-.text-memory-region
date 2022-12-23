std::string file_path = "file path here";

//Example of getting the .text memory section and returning it as an array of bytes
std::vector<uint8_t> get_text_section() {
	uintptr_t text_start = 0;
	uintptr_t text_end = 0;

	auto callback = [&](struct dl_phdr_info* info, size_t size, void* data) -> int
	{
		for (int i = 0; i < info->dlpi_phnum; ++i)
		{
			const auto& phdr = info->dlpi_phdr[i];
			if (phdr.p_type == PT_LOAD && phdr.p_flags == (PF_R | PF_X))
			{
				text_start = info->dlpi_addr + phdr.p_vaddr;
				text_end = text_start + phdr.p_memsz;
				return 1;
			}
		}
		return 0;
	};

	dl_iterate_phdr(callback, nullptr);

	std::size_t text_size = text_end - text_start;

	std::vector<uint8_t> text(text_size);

	memcpy(text.data(), reinterpret_cast<void*>(text_start), text_size);
	return text;
}

//Example of hashing text section
void hash_text_section()
{
	std::ifstream file(file_path, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error: Failed to open file " << file_path << std::endl;
	}

	std::string contents((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	std::size_t text_start = contents.find(".text\0");
	std::size_t text_end = contents.find(".data\0");
	if (text_start == std::string::npos || text_end == std::string::npos) {
		std::cerr << "Error: .text or .data section not found" << std::endl;
	}

	std::hash<std::string> hash_fn;
	std::size_t hash = hash_fn(contents.substr(text_start, text_end - text_start));

	std::cout << "Hash of .text section: 0x" << std::hex << hash << std::endl;

	getchar();
	exit(0);
}
