$sel = '\${' + 'created_date' + '}'

foreach ($file in Get-ChildItem . * -Recurse -Force | Select-String $sel -casesensitive)
{
    Write-Host "Processing " -ForegroundColor Red -NoNewline
    Write-Host $file.Path
    $gitCreated = git log --diff-filter=A --follow --pretty="format:%ci" -1 -- $file.Path
    Write-Host 'Created: ' + $gitCreated
    ((Get-Content -path $file.Path -Raw) -replace $sel,$gitCreated) | Set-Content -Path $file.Path
}
Write-Host "All files has been recoded" -ForegroundColor Green