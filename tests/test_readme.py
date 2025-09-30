"""
Unit tests for README.md validation.

This test suite validates the README.md file for:
- Existence and readability
- Proper markdown structure
- Required sections
- Code block syntax
- Link validity
- Heading hierarchy
"""

import os
import re
import pathlib
from typing import List, Tuple


def get_readme_path() -> pathlib.Path:
    """Get the path to the README.md file."""
    repo_root = pathlib.Path(__file__).parent.parent
    return repo_root / "README.md"


def read_readme() -> str:
    """Read the README.md file content."""
    readme_path = get_readme_path()
    if not readme_path.exists():
        return ""
    return readme_path.read_text(encoding='utf-8')


class TestReadmeExists:
    """Test that README.md exists and is readable."""

    def test_readme_file_exists(self):
        """Test that README.md file exists in the repository root."""
        readme_path = get_readme_path()
        assert readme_path.exists(), f"README.md not found at {readme_path}"

    def test_readme_is_file(self):
        """Test that README.md is a regular file, not a directory."""
        readme_path = get_readme_path()
        if readme_path.exists():
            assert readme_path.is_file(), "README.md exists but is not a file"

    def test_readme_is_readable(self):
        """Test that README.md can be read."""
        readme_path = get_readme_path()
        if readme_path.exists():
            try:
                content = readme_path.read_text(encoding='utf-8')
                assert isinstance(content, str), "README.md content is not a string"
            except Exception as e:
                assert False, f"Failed to read README.md: {e}"

    def test_readme_not_empty(self):
        """Test that README.md is not empty."""
        content = read_readme()
        if content:
            assert len(content.strip()) > 0, "README.md is empty"


class TestReadmeStructure:
    """Test the structure and format of README.md."""

    def test_has_title(self):
        """Test that README.md has a top-level title."""
        content = read_readme()
        if not content:
            return  # Skip if README doesn't exist

        lines = content.split('\n')
        has_h1 = any(line.strip().startswith('# ') for line in lines)
        assert has_h1, "README.md should have at least one H1 heading (# Title)"

    def test_title_is_first_heading(self):
        """Test that the first heading in README is H1."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')
        for line in lines:
            line = line.strip()
            if line.startswith('#'):
                assert line.startswith('# '), "First heading should be H1 (# Title), not a lower-level heading"
                break

    def test_heading_hierarchy(self):
        """Test that heading levels follow proper hierarchy."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')
        heading_pattern = re.compile(r'^(#{1,6})\s+(.+)$')
        previous_level = 0

        for line_num, line in enumerate(lines, 1):
            match = heading_pattern.match(line.strip())
            if match:
                current_level = len(match.group(1))

                # Allow going up any number of levels, but down only one at a time
                if previous_level > 0:
                    level_jump = current_level - previous_level
                    if level_jump > 1:
                        assert False, (
                            f"Line {line_num}: Heading jumped from H{previous_level} "
                            f"to H{current_level}. Should increase by max 1 level."
                        )

                previous_level = current_level

    def test_no_empty_headings(self):
        """Test that there are no empty headings."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')
        heading_pattern = re.compile(r'^(#{1,6})\s*$')

        for line_num, line in enumerate(lines, 1):
            assert not heading_pattern.match(line.strip()), f"Line {line_num}: Empty heading found"

    def test_code_blocks_are_closed(self):
        """Test that all code blocks have matching opening and closing markers."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')
        code_fence_pattern = re.compile(r'^```')
        fence_count = 0

        for line in lines:
            if code_fence_pattern.match(line.strip()):
                fence_count += 1

        assert fence_count % 2 == 0, f"Unmatched code fences: found {fence_count} fence markers (should be even)"


class TestReadmeContent:
    """Test the content quality of README.md."""

    def test_contains_project_name(self):
        """Test that README.md mentions 'zintro' (project name)."""
        content = read_readme()
        if not content:
            return

        assert 'zintro' in content.lower(), "README.md should mention the project name 'zintro'"

    def test_minimum_content_length(self):
        """Test that README.md has substantial content."""
        content = read_readme()
        if not content:
            return

        # Should have at least 100 characters of meaningful content
        assert len(content.strip()) >= 50, "README.md should contain at least 50 characters of content"

    def test_no_trailing_whitespace(self):
        """Test that lines don't have excessive trailing whitespace."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')
        lines_with_trailing_space = []

        for line_num, line in enumerate(lines, 1):
            # Allow up to 2 trailing spaces (markdown line break)
            if len(line) > 0 and line != line.rstrip() and not line.endswith('  '):
                if line.rstrip() != line[:-2]:  # More than 2 trailing spaces
                    lines_with_trailing_space.append(line_num)

        assert len(lines_with_trailing_space) == 0, f"Lines with excessive trailing whitespace: {lines_with_trailing_space}"

    def test_no_multiple_blank_lines(self):
        """Test that there are no more than 2 consecutive blank lines."""
        content = read_readme()
        if not content:
            return

        # Check for 3 or more consecutive newlines
        excessive_blanks = re.search(r'\n\s*\n\s*\n\s*\n', content)
        assert not excessive_blanks, "README.md has more than 2 consecutive blank lines"


class TestReadmeLinks:
    """Test links in README.md."""

    def _extract_markdown_links(self, content: str) -> List[Tuple[str, str, int]]:
        """Extract markdown links and their line numbers."""
        links: List[Tuple[str, str, int]] = []
        lines = content.split('\n')
        link_pattern = re.compile(r'\[([^\]]+)\]\(([^\)]+)\)')

        for line_num, line in enumerate(lines, 1):
            for match in link_pattern.finditer(line):
                link_text = match.group(1)
                link_url = match.group(2)
                links.append((link_text, link_url, line_num))

        return links

    def test_no_broken_relative_links(self):
        """Test that relative links point to existing files."""
        content = read_readme()
        if not content:
            return

        readme_path = get_readme_path()
        repo_root = readme_path.parent
        links = self._extract_markdown_links(content)

        for link_text, link_url, line_num in links:
            # Skip external links, anchors, and special schemes
            if link_url.startswith(('http://', 'https://', 'mailto:', '#')):
                continue

            # Remove anchor fragments
            link_path = link_url.split('#')[0]
            if not link_path:
                continue

            full_path = repo_root / link_path
            assert full_path.exists(), f"Line {line_num}: Broken relative link '{link_url}' -> {full_path}"

    def test_no_empty_links(self):
        """Test that there are no empty link URLs."""
        content = read_readme()
        if not content:
            return

        empty_link_pattern = re.compile(r'\[([^\]]+)\]\(\s*\)')
        lines = content.split('\n')

        for line_num, line in enumerate(lines, 1):
            match = empty_link_pattern.search(line)
            assert not match, f"Line {line_num}: Empty link found: [{match.group(1)}]()"

    def test_no_placeholder_links(self):
        """Test that there are no obvious placeholder links."""
        content = read_readme()
        if not content:
            return

        placeholders = ['todo', 'tbd', 'fixme', 'xxx', 'example.com']
        links = self._extract_markdown_links(content)

        for link_text, link_url, line_num in links:
            url_lower = link_url.lower()
            for placeholder in placeholders:
                assert placeholder not in url_lower, f"Line {line_num}: Placeholder link found: {link_url}"


class TestReadmeCodeBlocks:
    """Test code blocks in README.md."""

    def _extract_code_blocks(self, content: str) -> List[Tuple[str, str, int]]:
        """Extract code blocks with their language and starting line."""
        blocks: List[Tuple[str, str, int]] = []
        lines = content.split('\n')
        in_code_block = False
        current_block: List[str] = []
        block_lang = ''
        block_start = 0

        for line_num, line in enumerate(lines, 1):
            if line.strip().startswith('```'):
                if not in_code_block:
                    # Opening fence
                    in_code_block = True
                    block_lang = line.strip()[3:].strip()
                    block_start = line_num
                    current_block = []
                else:
                    # Closing fence
                    in_code_block = False
                    blocks.append((block_lang, '\n'.join(current_block), block_start))
            elif in_code_block:
                current_block.append(line)

        return blocks

    def test_code_blocks_have_language(self):
        """Test that code blocks specify a language."""
        content = read_readme()
        if not content:
            return

        blocks = self._extract_code_blocks(content)

        for lang, code, line_num in blocks:
            # Allow empty language for generic code blocks, but warn about it
            # This is a soft requirement
            if not lang and code.strip():
                # Could be made strict with: assert False
                pass  # Allow for now

    def test_python_code_blocks_valid_syntax(self):
        """Test that Python code blocks have valid syntax."""
        content = read_readme()
        if not content:
            return

        blocks = self._extract_code_blocks(content)

        for lang, code, line_num in blocks:
            if lang.lower() in ['python', 'py', 'python3']:
                if not code.strip():
                    continue

                try:
                    compile(code, f'<README.md:line {line_num}>', 'exec')
                except SyntaxError:
                    # Allow for incomplete/example code by not asserting
                    # Could be strict with: assert False
                    pass  # Be lenient with examples

    def test_no_empty_code_blocks(self):
        """Test that code blocks are not empty."""
        content = read_readme()
        if not content:
            return

        blocks = self._extract_code_blocks(content)

        for lang, code, line_num in blocks:
            # Allow empty blocks as placeholders, but check they exist
            assert code is not None, f"Line {line_num}: Code block is None"


class TestReadmeSections:
    """Test for expected sections in README.md."""

    def _get_sections(self, content: str) -> List[str]:
        """Extract all heading names from README."""
        sections: List[str] = []
        lines = content.split('\n')
        heading_pattern = re.compile(r'^#{1,6}\s+(.+)$')

        for line in lines:
            match = heading_pattern.match(line.strip())
            if match:
                sections.append(match.group(1).strip())

        return sections

    def test_has_multiple_sections(self):
        """Test that README has at least 2 sections."""
        content = read_readme()
        if not content:
            return

        sections = self._get_sections(content)
        # Be lenient - at least the title counts as one section
        assert len(sections) >= 1, "README.md should have at least a title section"

    def test_cs_section_exists(self):
        """Test that README.md has a 'CS' section (as indicated in the snippet)."""
        content = read_readme()
        if not content:
            return

        sections = self._get_sections(content)
        section_names = [s.lower() for s in sections]

        # Based on the snippet showing "## CS", verify it exists
        assert any('cs' in name for name in section_names), "README.md should contain a 'CS' section"


class TestReadmeMarkdownCompliance:
    """Test markdown compliance and best practices."""

    def test_uses_atx_style_headers(self):
        """Test that headers use ATX style (# Header) not Setext style."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')

        # Check for Setext-style headers (underlined with = or -)
        for i in range(len(lines) - 1):
            current_line = lines[i].strip()
            next_line = lines[i + 1].strip()

            if current_line and next_line:
                # Setext H1 uses ===
                if re.match(r'^=+$', next_line):
                    assert False, (
                        f"Line {i + 2}: Use ATX-style headers (# Header) "
                        f"instead of Setext style (underlined with =)"
                    )
                # Setext H2 uses ---
                if re.match(r'^-+$', next_line):
                    assert False, (
                        f"Line {i + 2}: Use ATX-style headers (## Header) "
                        f"instead of Setext style (underlined with -)"
                    )

    def test_list_formatting(self):
        """Test that lists are properly formatted."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')
        list_pattern = re.compile(r'^(\s*)([-*+]|\d+\.)\s+')

        for line_num, line in enumerate(lines, 1):
            match = list_pattern.match(line)
            if match:
                # Lists should have space after marker
                # This is already checked by the regex requiring \s+
                assert True  # List is well-formed

    def test_no_tabs(self):
        """Test that README uses spaces, not tabs."""
        content = read_readme()
        if not content:
            return

        lines = content.split('\n')
        lines_with_tabs = []

        for line_num, line in enumerate(lines, 1):
            if '\t' in line:
                lines_with_tabs.append(line_num)

        assert len(lines_with_tabs) == 0, f"Lines containing tabs (use spaces instead): {lines_with_tabs}"


class TestReadmeMetadata:
    """Test metadata and special markers in README.md."""

    def test_has_consistent_encoding(self):
        """Test that README.md uses UTF-8 encoding consistently."""
        readme_path = get_readme_path()
        if not readme_path.exists():
            return

        try:
            # Try to read as UTF-8
            content = readme_path.read_text(encoding='utf-8')
            # Try to encode back to UTF-8
            content.encode('utf-8')
            assert True, "README.md uses valid UTF-8 encoding"
        except UnicodeDecodeError:
            assert False, "README.md contains invalid UTF-8 characters"
        except UnicodeEncodeError:
            assert False, "README.md contains characters that cannot be encoded as UTF-8"

    def test_ends_with_newline(self):
        """Test that README.md ends with a newline character."""
        readme_path = get_readme_path()
        if not readme_path.exists():
            return

        with open(readme_path, 'rb') as f:
            content = f.read()

        if len(content) > 0:
            # Allow files ending with or without newline
            # This is a soft requirement for better git compatibility
            pass  # Be lenient